"""Install C/C++ header files to the Python include directory."""

from packaging.command.cmd import Command


# XXX force is never used
class install_headers(Command):

    description = "install C/C++ header files"

    user_options = [('install-dir=', 'd',
                     "directory to install header files to"),
                    ('force', 'f',
                     "force installation (overwrite existing files)"),
                   ]

    boolean_options = ['force']

    def initialize_options(self):
        self.install_dir = None
        self.force = False
        self.outfiles = []

    def finalize_options(self):
        self.set_undefined_options('install_dist',
                                   ('install_headers', 'install_dir'),
                                   'force')

    def run(self):
        headers = self.distribution.headers
        if not headers:
            return

        self.mkpath(self.install_dir)
        for header in headers:
            out = self.copy_file(header, self.install_dir)[0]
            self.outfiles.append(out)

    def get_inputs(self):
        return self.distribution.headers or []

    def get_outputs(self):
        return self.outfiles
