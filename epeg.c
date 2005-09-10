#include <ruby.h>
#include <intern.h>
#include <Epeg.h>

VALUE cEpeg;

static VALUE rb_epeg_thumbnail(klass, fname, x, y)
	VALUE klass, fname, x, y;
{
	Epeg_Image *image = NULL;
	unsigned char *data = NULL;
	int size = 0;
	VALUE retval;

	Check_Type(fname, T_STRING);
	if(StringValueCStr(fname)[0] != '/') {
		fname = rb_file_s_expand_path(1, &fname);
	}

	image = epeg_file_open(StringValueCStr(fname));
	epeg_decode_size_set(image, NUM2INT(x), NUM2INT(y));
	epeg_memory_output_set(image, &data, &size);
	if(epeg_encode(image) != 0) {
		rb_raise(rb_eStandardError, "Can't encode image");
	}
	epeg_close(image);
	retval = rb_str_new(data, size);
	free(data);
	return retval;
}

static VALUE
rb_epeg_new(klass, fname)
	VALUE klass, fname;
{
	VALUE retval;
	Epeg_Image *image;
	image = epeg_file_open(StringValueCStr(fname));
	retval = Data_Wrap_Struct(klass, NULL, epeg_close, image);
	rb_obj_call_init(retval, 0, NULL);
	return retval;
}

void Init_epeg () {
	cEpeg = rb_define_class("Epeg", rb_cObject);
	rb_define_singleton_method(cEpeg, "thumbnail", rb_epeg_thumbnail, 3);
	rb_define_singleton_method(cEpeg, "new", rb_epeg_new, 1);
}
