#include <ruby.h>
#include <intern.h>
#include <Epeg.h>

VALUE cEpeg;
ID idvalid;
ID idsize;
ID idclosed;

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
	retval = Data_Wrap_Struct(klass, NULL, NULL, image);
	rb_ivar_set(retval, idvalid, Qtrue);
	rb_obj_call_init(retval, 0, NULL);
	return retval;
}

static VALUE
rb_epeg_get_size(obj)
	VALUE obj;
{
	int x, y;
	Epeg_Image *image;
	VALUE retval;
	retval = rb_ivar_get(obj, idsize);
	if(RTEST(retval))
		return retval;
	Data_Get_Struct(obj, Epeg_Image, image);
	epeg_size_get(image, &x, &y);
	retval = rb_ary_new3(2, INT2NUM(x), INT2NUM(y));
	rb_ivar_set(obj, idsize, retval);
	return retval;
}

static VALUE
rb_epeg_finish(obj)
	VALUE obj;
{
	Epeg_Image *image;
	VALUE retval;
	if(OBJ_FROZEN(obj)) {
		rb_raise(rb_eStandardError, "Epeg finished");
	}
	if(!RTEST(rb_ivar_get(obj, idvalid))) {
		rb_raise(rb_eStandardError, "I don't know what to do");
	}
	Data_Get_Struct(obj, Epeg_Image, image);
	epeg_close(image);
	OBJ_FREEZE(obj);
	return obj;
}

void Init_epeg () {
	idclosed = rb_intern("closed");
	idvalid = rb_intern("valid");
	idsize = rb_intern("size");
	cEpeg = rb_define_class("Epeg", rb_cObject);
	rb_define_singleton_method(cEpeg, "thumbnail", rb_epeg_thumbnail, 3);
	rb_define_singleton_method(cEpeg, "new", rb_epeg_new, 1);
	rb_define_method(cEpeg, "size", rb_epeg_get_size, 0);
	rb_define_method(cEpeg, "finish", rb_epeg_finish, 0);
}
