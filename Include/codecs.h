#ifndef Py_CODECREGISTRY_H
#define Py_CODECREGISTRY_H
#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------

   Python Codec Registry and support functions


Written by Marc-Andre Lemburg (mal@lemburg.com).

Copyright (c) Corporation for National Research Initiatives.

   ------------------------------------------------------------------------ */

/* Register a new codec search function.

   As side effect, this tries to load the encodings package, if not
   yet done, to make sure that it is always first in the list of
   search functions.

   The search_function's refcount is incremented by this function. */

PyAPI_FUNC(int) PyCodec_Register(
       PyObject *search_function
       );

/* Codec register lookup API.

   Looks up the given encoding and returns a tuple (encoder, decoder,
   stream reader, stream writer) of functions which implement the
   different aspects of processing the encoding.

   The encoding string is looked up converted to all lower-case
   characters. This makes encodings looked up through this mechanism
   effectively case-insensitive.

   If no codec is found, a KeyError is set and NULL returned. 

   As side effect, this tries to load the encodings package, if not
   yet done. This is part of the lazy load strategy for the encodings
   package.

 */

PyAPI_FUNC(PyObject *) _PyCodec_Lookup(
       const char *encoding
       );

/* Generic codec based encoding API.

   object is passed through the encoder function found for the given
   encoding using the error handling method defined by errors. errors
   may be NULL to use the default method defined for the codec.
   
   Raises a LookupError in case no encoder can be found.

 */

PyAPI_FUNC(PyObject *) PyCodec_Encode(
       PyObject *object,
       const char *encoding,
       const char *errors
       );

/* Generic codec based decoding API.

   object is passed through the decoder function found for the given
   encoding using the error handling method defined by errors. errors
   may be NULL to use the default method defined for the codec.
   
   Raises a LookupError in case no encoder can be found.

 */

PyAPI_FUNC(PyObject *) PyCodec_Decode(
       PyObject *object,
       const char *encoding,
       const char *errors
       );

/* --- Codec Lookup APIs -------------------------------------------------- 

   All APIs return a codec object with incremented refcount and are
   based on _PyCodec_Lookup().  The same comments w/r to the encoding
   name also apply to these APIs.

*/

/* Get an encoder function for the given encoding. */

PyAPI_FUNC(PyObject *) PyCodec_Encoder(
       const char *encoding
       );

/* Get a decoder function for the given encoding. */

PyAPI_FUNC(PyObject *) PyCodec_Decoder(
       const char *encoding
       );

/* Get a StreamReader factory function for the given encoding. */

PyAPI_FUNC(PyObject *) PyCodec_StreamReader(
       const char *encoding,
       PyObject *stream,
       const char *errors
       );

/* Get a StreamWriter factory function for the given encoding. */

PyAPI_FUNC(PyObject *) PyCodec_StreamWriter(
       const char *encoding,
       PyObject *stream,
       const char *errors
       );

#ifdef __cplusplus
}
#endif
#endif /* !Py_CODECREGISTRY_H */
