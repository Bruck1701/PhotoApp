#include <nokogiri.h>

VALUE cNokogiriHtmlSaxPushParser;

/*
 * call-seq:
 *  native_write(chunk, last_chunk)
 *
 * Write +chunk+ to PushParser. +last_chunk+ triggers the end_document handle
 */
static VALUE
native_write(VALUE self, VALUE _chunk, VALUE _last_chunk)
{
  xmlParserCtxtPtr ctx;
  const char *chunk = NULL;
  int size = 0;
  int status = 0;
  libxmlStructuredErrorHandlerState handler_state;

  Data_Get_Struct(self, xmlParserCtxt, ctx);

  if (Qnil != _chunk) {
    chunk = StringValuePtr(_chunk);
    size = (int)RSTRING_LEN(_chunk);
  }

  Nokogiri_structured_error_func_save_and_set(&handler_state, NULL, NULL);

  status = htmlParseChunk(ctx, chunk, size, Qtrue == _last_chunk ? 1 : 0);

  Nokogiri_structured_error_func_restore(&handler_state);

  if ((status != 0) && !(ctx->options & XML_PARSE_RECOVER)) {
    // TODO: there appear to be no tests for this block
    xmlErrorPtr e = xmlCtxtGetLastError(ctx);
    Nokogiri_error_raise(NULL, e);
  }

  return self;
}

/*
 * call-seq:
 *  initialize_native(xml_sax, filename)
 *
 * Initialize the push parser with +xml_sax+ using +filename+
 */
static VALUE
initialize_native(VALUE self, VALUE _xml_sax, VALUE _filename,
                  VALUE encoding)
{
  htmlSAXHandlerPtr sax;
  const char *filename = NULL;
  htmlParserCtxtPtr ctx;
  xmlCharEncoding enc = XML_CHAR_ENCODING_NONE;

  Data_Get_Struct(_xml_sax, xmlSAXHandler, sax);

  if (_filename != Qnil) { filename = StringValueCStr(_filename); }

  if (!NIL_P(encoding)) {
    enc = xmlParseCharEncoding(StringValueCStr(encoding));
    if (enc == XML_CHAR_ENCODING_ERROR) {
      rb_raise(rb_eArgError, "Unsupported Encoding");
    }
  }

  ctx = htmlCreatePushParserCtxt(
          sax,
          NULL,
          NULL,
          0,
          filename,
          enc
        );
  if (ctx == NULL) {
    rb_raise(rb_eRuntimeError, "Could not create a parser context");
  }

  ctx->userData = NOKOGIRI_SAX_TUPLE_NEW(ctx, self);

  ctx->sax2 = 1;
  DATA_PTR(self) = ctx;
  return self;
}

void
noko_init_html_sax_push_parser()
{
  assert(cNokogiriXmlSaxPushParser);
  cNokogiriHtmlSaxPushParser = rb_define_class_under(mNokogiriHtmlSax, "PushParser", cNokogiriXmlSaxPushParser);

  rb_define_private_method(cNokogiriHtmlSaxPushParser, "initialize_native", initialize_native, 3);
  rb_define_private_method(cNokogiriHtmlSaxPushParser, "native_write", native_write, 2);
}
