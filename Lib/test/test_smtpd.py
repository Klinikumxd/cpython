from unittest import TestCase
from test import support, mock_socket
import socket
import io
import smtpd
import asyncore


class DummyServer(smtpd.SMTPServer):
    def __init__(self, *args):
        smtpd.SMTPServer.__init__(self, *args)
        self.messages = []

    def process_message(self, peer, mailfrom, rcpttos, data):
        self.messages.append((peer, mailfrom, rcpttos, data))
        if data == 'return status':
            return '250 Okish'

class DummyDispatcherBroken(Exception):
    pass

class BrokenDummyServer(DummyServer):
    def listen(self, num):
        raise DummyDispatcherBroken()

class SMTPDChannelTest(TestCase):
    def setUp(self):
        smtpd.socket = asyncore.socket = mock_socket
        self.debug = smtpd.DEBUGSTREAM = io.StringIO()
        self.server = DummyServer('a', 'b')
        conn, addr = self.server.accept()
        self.channel = smtpd.SMTPChannel(self.server, conn, addr)

    def tearDown(self):
        asyncore.socket = smtpd.socket = socket

    def write_line(self, line):
        self.channel.socket.queue_recv(line)
        self.channel.handle_read()

    def test_broken_connect(self):
        self.assertRaises(DummyDispatcherBroken, BrokenDummyServer, 'a', 'b')

    def test_server_accept(self):
        self.server.handle_accept()

    def test_missing_data(self):
        self.write_line(b'')
        self.assertEqual(self.channel.socket.last,
                         b'500 Error: bad syntax\r\n')

    def test_EHLO_not_implemented(self):
        self.write_line(b'EHLO test.example')
        self.assertEqual(self.channel.socket.last,
                         b'502 Error: command "EHLO" not implemented\r\n')

    def test_HELO(self):
        name = smtpd.socket.getfqdn()
        self.write_line(b'HELO test.example')
        self.assertEqual(self.channel.socket.last,
                         '250 {}\r\n'.format(name).encode('ascii'))

    def test_HELO_bad_syntax(self):
        self.write_line(b'HELO')
        self.assertEqual(self.channel.socket.last,
                         b'501 Syntax: HELO hostname\r\n')

    def test_HELO_duplicate(self):
        self.write_line(b'HELO test.example')
        self.write_line(b'HELO test.example')
        self.assertEqual(self.channel.socket.last,
                         b'503 Duplicate HELO/EHLO\r\n')

    def test_NOOP(self):
        self.write_line(b'NOOP')
        self.assertEqual(self.channel.socket.last, b'250 Ok\r\n')

    def test_NOOP_bad_syntax(self):
        self.write_line(b'NOOP hi')
        self.assertEqual(self.channel.socket.last,
                         b'501 Syntax: NOOP\r\n')

    def test_QUIT(self):
        self.write_line(b'QUIT')
        self.assertEqual(self.channel.socket.last, b'221 Bye\r\n')

    def test_QUIT_arg_ignored(self):
        self.write_line(b'QUIT bye bye')
        self.assertEqual(self.channel.socket.last, b'221 Bye\r\n')

    def test_bad_state(self):
        self.channel.smtp_state = 'BAD STATE'
        self.write_line(b'HELO')
        self.assertEqual(self.channel.socket.last,
                         b'451 Internal confusion\r\n')

    def test_need_MAIL(self):
        self.write_line(b'RCPT to:spam@example')
        self.assertEqual(self.channel.socket.last,
            b'503 Error: need MAIL command\r\n')

    def test_MAIL_syntax(self):
        self.write_line(b'MAIL from eggs@example')
        self.assertEqual(self.channel.socket.last,
            b'501 Syntax: MAIL FROM:<address>\r\n')

    def test_MAIL_missing_from(self):
        self.write_line(b'MAIL from:')
        self.assertEqual(self.channel.socket.last,
            b'501 Syntax: MAIL FROM:<address>\r\n')

    def test_MAIL_chevrons(self):
        self.write_line(b'MAIL from:<eggs@example>')
        self.assertEqual(self.channel.socket.last, b'250 Ok\r\n')

    def test_nested_MAIL(self):
        self.write_line(b'MAIL from:eggs@example')
        self.write_line(b'MAIL from:spam@example')
        self.assertEqual(self.channel.socket.last,
            b'503 Error: nested MAIL command\r\n')

    def test_need_RCPT(self):
        self.write_line(b'MAIL From:eggs@example')
        self.write_line(b'DATA')
        self.assertEqual(self.channel.socket.last,
            b'503 Error: need RCPT command\r\n')

    def test_RCPT_syntax(self):
        self.write_line(b'MAIL From:eggs@example')
        self.write_line(b'RCPT to eggs@example')
        self.assertEqual(self.channel.socket.last,
            b'501 Syntax: RCPT TO: <address>\r\n')

    def test_data_dialog(self):
        self.write_line(b'MAIL From:eggs@example')
        self.assertEqual(self.channel.socket.last, b'250 Ok\r\n')
        self.write_line(b'RCPT To:spam@example')
        self.assertEqual(self.channel.socket.last, b'250 Ok\r\n')

        self.write_line(b'DATA')
        self.assertEqual(self.channel.socket.last,
            b'354 End data with <CR><LF>.<CR><LF>\r\n')
        self.write_line(b'data\r\nmore\r\n.')
        self.assertEqual(self.channel.socket.last, b'250 Ok\r\n')
        self.assertEqual(self.server.messages[-1],
            ('peer', 'eggs@example', ['spam@example'], 'data\nmore'))

    def test_DATA_syntax(self):
        self.write_line(b'MAIL From:eggs@example')
        self.write_line(b'RCPT To:spam@example')
        self.write_line(b'DATA spam')
        self.assertEqual(self.channel.socket.last, b'501 Syntax: DATA\r\n')

    def test_multiple_RCPT(self):
        self.write_line(b'MAIL From:eggs@example')
        self.write_line(b'RCPT To:spam@example')
        self.write_line(b'RCPT To:ham@example')
        self.write_line(b'DATA')
        self.write_line(b'data\r\n.')
        self.assertEqual(self.server.messages[-1],
            ('peer', 'eggs@example', ['spam@example','ham@example'], 'data'))

    def test_manual_status(self):
        self.write_line(b'MAIL From:eggs@example')
        self.write_line(b'RCPT To:spam@example')
        self.write_line(b'DATA')
        self.write_line(b'return status\r\n.')
        self.assertEqual(self.channel.socket.last, b'250 Okish\r\n')

    def test_RSET(self):
        self.write_line(b'MAIL From:eggs@example')
        self.write_line(b'RCPT To:spam@example')
        self.write_line(b'RSET')
        self.assertEqual(self.channel.socket.last, b'250 Ok\r\n')
        self.write_line(b'MAIL From:foo@example')
        self.write_line(b'RCPT To:eggs@example')
        self.write_line(b'DATA')
        self.write_line(b'data\r\n.')
        self.assertEqual(self.server.messages[0],
            ('peer', 'foo@example', ['eggs@example'], 'data'))

    def test_RSET_syntax(self):
        self.write_line(b'RSET hi')
        self.assertEqual(self.channel.socket.last, b'501 Syntax: RSET\r\n')


def test_main():
    support.run_unittest(SMTPDChannelTest)

if __name__ == "__main__":
    test_main()
