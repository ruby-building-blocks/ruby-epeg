#include <ruby.h>
#include <ruby/intern.h>
#include <Epeg.h>

VALUE cEpeg;
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
	if(!image) {
		rb_raise(rb_eStandardError, "Can't open image");
	}
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
	if(!image) {
		rb_raise(rb_eStandardError, "Can't open image");
	}
	retval = Data_Wrap_Struct(klass, NULL, NULL, image);
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
rb_epeg_set_output_size(obj, x, y)
	VALUE obj, x, y;
{
	Epeg_Image *image;
	VALUE retval;
	if(OBJ_FROZEN(obj)) {
		rb_raise(rb_eStandardError, "can't set size now");
	}
	Data_Get_Struct(obj, Epeg_Image, image);
	epeg_decode_size_set(image, NUM2INT(x), NUM2INT(y));
	return Qnil;
}

static VALUE
rb_epeg_finish(argc, args, obj)
	int argc;
	VALUE *args;
	VALUE obj;
{
	Epeg_Image *image;
	VALUE retval;
	unsigned char *data;
	int size;

	if(OBJ_FROZEN(obj)) {
		rb_raise(rb_eStandardError, "Epeg finished");
	}

	Data_Get_Struct(obj, Epeg_Image, image);

	if(argc == 0) { // Return as string
		epeg_memory_output_set(image, &data, &size);
	} else if(argc == 1) { // Filename
		Check_Type(args[0], T_STRING);
		epeg_file_output_set(image, RSTRING_PTR(args[0]));
	} else {
		rb_raise(rb_eRuntimeError, "invalid arguments");
	}

	if(epeg_encode(image) != 0) {
		rb_raise(rb_eStandardError, "Can't encode image");
	}
	epeg_close(image);
	OBJ_FREEZE(obj);
	if(argc == 0) {
		retval = rb_str_new(data, size);
		free(data);
		return retval;
	} else {
		return Qnil;
	}
}

void Init_epeg () {
	idclosed = rb_intern("closed");
	idsize = rb_intern("size");
	cEpeg = rb_define_class("Epeg", rb_cObject);
	rb_define_singleton_method(cEpeg, "thumbnail", rb_epeg_thumbnail, 3);
	rb_define_singleton_method(cEpeg, "new", rb_epeg_new, 1);
	rb_define_method(cEpeg, "size", rb_epeg_get_size, 0);
	rb_define_method(cEpeg, "set_output_size", rb_epeg_set_output_size, 2);
	rb_define_method(cEpeg, "finish", rb_epeg_finish, -1);
}
