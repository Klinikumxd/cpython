"""
This module contains the core classes of version 2.0 of SAX for Python.
This file provides only default classes with absolutely minimum
functionality, from which drivers and applications can be subclassed.

Many of these classes are empty and are included only as documentation
of the interfaces.

$Id$
"""

version = '2.0beta'

#============================================================================
#
# HANDLER INTERFACES
#
#============================================================================

# ===== ErrorHandler =====

class ErrorHandler:
    """Basic interface for SAX error handlers. If you create an object
    that implements this interface, then register the object with your
    Parser, the parser will call the methods in your object to report
    all warnings and errors. There are three levels of errors
    available: warnings, (possibly) recoverable errors, and
    unrecoverable errors. All methods take a SAXParseException as the
    only parameter."""

    def error(self, exception):
        "Handle a recoverable error."
        raise exception

    def fatalError(self, exception):
        "Handle a non-recoverable error."
        raise exception

    def warning(self, exception):
        "Handle a warning."   
        print exception

# ===== CONTENTHANDLER =====

class ContentHandler:
    """Interface for receiving logical document content events.

    This is the main callback interface in SAX, and the one most
    important to applications. The order of events in this interface
    mirrors the order of the information in the document."""

    def __init__(self):
        self._locator = None
    
    def setDocumentLocator(self, locator):
        """Called by the parser to give the application a locator for
        locating the origin of document events.

        SAX parsers are strongly encouraged (though not absolutely
        required) to supply a locator: if it does so, it must supply
        the locator to the application by invoking this method before
        invoking any of the other methods in the DocumentHandler
        interface.

        The locator allows the application to determine the end
        position of any document-related event, even if the parser is
        not reporting an error. Typically, the application will use
        this information for reporting its own errors (such as
        character content that does not match an application's
        business rules). The information returned by the locator is
        probably not sufficient for use with a search engine.
        
        Note that the locator will return correct information only
        during the invocation of the events in this interface. The
        application should not attempt to use it at any other time."""
        self._locator = locator        

    def startDocument(self):
        """Receive notification of the beginning of a document.
        
        The SAX parser will invoke this method only once, before any
        other methods in this interface or in DTDHandler (except for
        setDocumentLocator)."""

    def endDocument(self):
        """Receive notification of the end of a document.
        
        The SAX parser will invoke this method only once, and it will
        be the last method invoked during the parse. The parser shall
        not invoke this method until it has either abandoned parsing
        (because of an unrecoverable error) or reached the end of
        input."""

    def startPrefixMapping(self, prefix, uri):
        """Begin the scope of a prefix-URI Namespace mapping.
        
        The information from this event is not necessary for normal
        Namespace processing: the SAX XML reader will automatically
        replace prefixes for element and attribute names when the
        http://xml.org/sax/features/namespaces feature is true (the
        default).
        
        There are cases, however, when applications need to use
        prefixes in character data or in attribute values, where they
        cannot safely be expanded automatically; the
        start/endPrefixMapping event supplies the information to the
        application to expand prefixes in those contexts itself, if
        necessary.

        Note that start/endPrefixMapping events are not guaranteed to
        be properly nested relative to each-other: all
        startPrefixMapping events will occur before the corresponding
        startElement event, and all endPrefixMapping events will occur
        after the corresponding endElement event, but their order is
        not guaranteed."""

    def endPrefixMapping(self, prefix):
        """End the scope of a prefix-URI mapping.
        
        See startPrefixMapping for details. This event will always
        occur after the corresponding endElement event, but the order
        of endPrefixMapping events is not otherwise guaranteed."""

    def startElement(self, name, attrs):
        """Signals the start of an element in non-namespace mode.

        The name parameter contains the raw XML 1.0 name of the
        element type as a string and the attrs parameter holds an
        instance of the Attributes class containing the attributes of
        the element."""

    def endElement(self, name):
        """Signals the end of an element in non-namespace mode.

        The name parameter contains the name of the element type, just
        as with the startElement event."""

    def startElementNS(self, name, qname, attrs):
        """Signals the start of an element in namespace mode.

        The name parameter contains the name of the element type as a
        (uri, localname) tuple, the qname parameter the raw XML 1.0
        name used in the source document, and the attrs parameter
        holds an instance of the Attributes class containing the
        attributes of the element."""

    def endElementNS(self, name, qname):
        """Signals the end of an element in namespace mode.

        The name parameter contains the name of the element type, just
        as with the startElementNS event."""
        
    def characters(self, content):
        """Receive notification of character data.
        
        The Parser will call this method to report each chunk of
        character data. SAX parsers may return all contiguous
        character data in a single chunk, or they may split it into
        several chunks; however, all of the characters in any single
        event must come from the same external entity so that the
        Locator provides useful information."""

    def ignorableWhitespace(self, whitespace):
        """Receive notification of ignorable whitespace in element content.
        
        Validating Parsers must use this method to report each chunk
        of ignorable whitespace (see the W3C XML 1.0 recommendation,
        section 2.10): non-validating parsers may also use this method
        if they are capable of parsing and using content models.
        
        SAX parsers may return all contiguous whitespace in a single
        chunk, or they may split it into several chunks; however, all
        of the characters in any single event must come from the same
        external entity, so that the Locator provides useful
        information.
        
        The application must not attempt to read from the array
        outside of the specified range."""

    def processingInstruction(self, target, data):
        """Receive notification of a processing instruction.
        
        The Parser will invoke this method once for each processing
        instruction found: note that processing instructions may occur
        before or after the main document element.

        A SAX parser should never report an XML declaration (XML 1.0,
        section 2.8) or a text declaration (XML 1.0, section 4.3.1)
        using this method."""

    def skippedEntity(self, name):
        """Receive notification of a skipped entity.
        
        The Parser will invoke this method once for each entity
        skipped. Non-validating processors may skip entities if they
        have not seen the declarations (because, for example, the
        entity was declared in an external DTD subset). All processors
        may skip external entities, depending on the values of the
        http://xml.org/sax/features/external-general-entities and the
        http://xml.org/sax/features/external-parameter-entities
        properties."""
        
#============================================================================
#
# CORE FEATURES
#
#============================================================================

feature_namespaces = "http://xml.org/sax/features/namespaces"
# true: Perform Namespace processing (default).
# false: Optionally do not perform Namespace processing
#        (implies namespace-prefixes).
# access: (parsing) read-only; (not parsing) read/write

feature_namespace_prefixes = "http://xml.org/sax/features/namespace-prefixes"
# true: Report the original prefixed names and attributes used for Namespace
#       declarations.
# false: Do not report attributes used for Namespace declarations, and
#        optionally do not report original prefixed names (default).
# access: (parsing) read-only; (not parsing) read/write

feature_string_interning = "http://xml.org/sax/features/string-interning"
# true: All element names, prefixes, attribute names, Namespace URIs, and
#       local names are interned using the built-in intern function.
# false: Names are not necessarily interned, although they may be (default).
# access: (parsing) read-only; (not parsing) read/write

feature_validation = "http://xml.org/sax/features/validation"
# true: Report all validation errors (implies external-general-entities and
#       external-parameter-entities).
# false: Do not report validation errors.
# access: (parsing) read-only; (not parsing) read/write

feature_external_ges = "http://xml.org/sax/features/external-general-entities"
# true: Include all external general (text) entities.
# false: Do not include external general entities.
# access: (parsing) read-only; (not parsing) read/write

feature_external_pes = "http://xml.org/sax/features/external-parameter-entities"
# true: Include all external parameter entities, including the external
#       DTD subset.
# false: Do not include any external parameter entities, even the external
#        DTD subset.
# access: (parsing) read-only; (not parsing) read/write

all_features = [feature_namespaces,
                feature_namespace_prefixes,
                feature_string_interning,
                feature_validation,
                feature_external_ges,
                feature_external_pes]


#============================================================================
#
# CORE PROPERTIES
#
#============================================================================

property_lexical_handler = "http://xml.org/sax/properties/lexical-handler"
# data type: xml.sax.sax2lib.LexicalHandler
# description: An optional extension handler for lexical events like comments.
# access: read/write

property_declaration_handler = "http://xml.org/sax/properties/declaration-handler"
# data type: xml.sax.sax2lib.DeclHandler
# description: An optional extension handler for DTD-related events other
#              than notations and unparsed entities.
# access: read/write

property_dom_node = "http://xml.org/sax/properties/dom-node"
# data type: org.w3c.dom.Node
# description: When parsing, the current DOM node being visited if this is
#              a DOM iterator; when not parsing, the root DOM node for
#              iteration.
# access: (parsing) read-only; (not parsing) read/write

property_xml_string = "http://xml.org/sax/properties/xml-string"
# data type: String
# description: The literal string of characters that was the source for
#              the current event.
# access: read-only

all_properties = [property_lexical_handler,
                  property_dom_node,
                  property_declaration_handler,
                  property_xml_string]
