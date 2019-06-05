#include "mlvalues.h"
#include "prims.h"
extern value alloc_dummy();
extern value update_dummy();
extern value array_get_addr();
extern value array_get_float();
extern value array_get();
extern value array_set_addr();
extern value array_set_float();
extern value array_set();
extern value array_unsafe_get_float();
extern value array_unsafe_get();
extern value array_unsafe_set_addr();
extern value array_unsafe_set_float();
extern value array_unsafe_set();
extern value make_vect();
extern value make_array();
extern value compare();
extern value equal();
extern value notequal();
extern value lessthan();
extern value lessequal();
extern value greaterthan();
extern value greaterequal();
extern value output_value();
extern value output_value_to_string();
extern value output_value_to_buffer();
extern value format_float();
extern value float_of_string();
extern value int_of_float();
extern value float_of_int();
extern value neg_float();
extern value abs_float();
extern value add_float();
extern value sub_float();
extern value mul_float();
extern value div_float();
extern value exp_float();
extern value floor_float();
extern value fmod_float();
extern value frexp_float();
extern value ldexp_float();
extern value log_float();
extern value log10_float();
extern value modf_float();
extern value sqrt_float();
extern value power_float();
extern value sin_float();
extern value sinh_float();
extern value cos_float();
extern value cosh_float();
extern value tan_float();
extern value tanh_float();
extern value asin_float();
extern value acos_float();
extern value atan_float();
extern value atan2_float();
extern value ceil_float();
extern value eq_float();
extern value neq_float();
extern value le_float();
extern value lt_float();
extern value ge_float();
extern value gt_float();
extern value float_of_bytes();
extern value classify_float();
extern value gc_stat();
extern value gc_counters();
extern value gc_get();
extern value gc_set();
extern value gc_minor();
extern value gc_major();
extern value gc_full_major();
extern value gc_major_slice();
extern value gc_compaction();
extern value hash_univ_param();
extern value input_value();
extern value input_value_from_string();
extern value marshal_data_size();
extern value int_of_string();
extern value format_int();
extern value int32_neg();
extern value int32_add();
extern value int32_sub();
extern value int32_mul();
extern value int32_div();
extern value int32_mod();
extern value int32_and();
extern value int32_or();
extern value int32_xor();
extern value int32_shift_left();
extern value int32_shift_right();
extern value int32_shift_right_unsigned();
extern value int32_of_int();
extern value int32_to_int();
extern value int32_of_float();
extern value int32_to_float();
extern value int32_format();
extern value int32_of_string();
extern value int64_neg();
extern value int64_add();
extern value int64_sub();
extern value int64_mul();
extern value int64_div();
extern value int64_mod();
extern value int64_and();
extern value int64_or();
extern value int64_xor();
extern value int64_shift_left();
extern value int64_shift_right();
extern value int64_shift_right_unsigned();
extern value int64_of_int();
extern value int64_to_int();
extern value int64_of_float();
extern value int64_to_float();
extern value int64_of_int32();
extern value int64_to_int32();
extern value int64_of_nativeint();
extern value int64_to_nativeint();
extern value int64_format();
extern value int64_of_string();
extern value int64_bits_of_float();
extern value int64_float_of_bits();
extern value nativeint_neg();
extern value nativeint_add();
extern value nativeint_sub();
extern value nativeint_mul();
extern value nativeint_div();
extern value nativeint_mod();
extern value nativeint_and();
extern value nativeint_or();
extern value nativeint_xor();
extern value nativeint_shift_left();
extern value nativeint_shift_right();
extern value nativeint_shift_right_unsigned();
extern value nativeint_of_int();
extern value nativeint_to_int();
extern value nativeint_of_float();
extern value nativeint_to_float();
extern value nativeint_of_int32();
extern value nativeint_to_int32();
extern value nativeint_format();
extern value nativeint_of_string();
extern value caml_open_descriptor_in();
extern value caml_open_descriptor_out();
extern value caml_out_channels_list();
extern value channel_descriptor();
extern value caml_close_channel();
extern value caml_channel_size();
extern value caml_channel_size_64();
extern value caml_set_binary_mode();
extern value caml_flush_partial();
extern value caml_flush();
extern value caml_output_char();
extern value caml_output_int();
extern value caml_output_partial();
extern value caml_output();
extern value caml_seek_out();
extern value caml_seek_out_64();
extern value caml_pos_out();
extern value caml_pos_out_64();
extern value caml_input_char();
extern value caml_input_int();
extern value caml_input();
extern value caml_seek_in();
extern value caml_seek_in_64();
extern value caml_pos_in();
extern value caml_pos_in_64();
extern value caml_input_scan_line();
extern value lex_engine();
extern value md5_string();
extern value md5_chan();
extern value get_global_data();
extern value reify_bytecode();
extern value realloc_global();
extern value get_current_environment();
extern value invoke_traced_function();
extern value static_alloc();
extern value static_free();
extern value static_resize();
extern value obj_is_block();
extern value obj_tag();
extern value obj_set_tag();
extern value obj_block();
extern value obj_dup();
extern value obj_truncate();
extern value parse_engine();
extern value install_signal_handler();
extern value ml_string_length();
extern value create_string();
extern value string_get();
extern value string_set();
extern value string_equal();
extern value string_notequal();
extern value blit_string();
extern value fill_string();
extern value is_printable();
extern value bitvect_test();
extern value sys_exit();
extern value sys_open();
extern value sys_close();
extern value sys_file_exists();
extern value sys_remove();
extern value sys_rename();
extern value sys_chdir();
extern value sys_getcwd();
extern value sys_getenv();
extern value sys_get_argv();
extern value sys_system_command();
extern value sys_time();
extern value sys_random_seed();
extern value sys_get_config();
extern value terminfo_setup();
extern value terminfo_backup();
extern value terminfo_standout();
extern value terminfo_resume();
extern value register_named_value();
extern value weak_create();
extern value weak_set();
extern value weak_get();
extern value weak_get_copy();
extern value weak_check();
extern value final_register();
extern value ensure_stack_capacity();
extern value dynlink_open_lib();
extern value dynlink_close_lib();
extern value dynlink_lookup_symbol();
extern value dynlink_add_primitive();
extern value dynlink_get_current_libs();
c_primitive builtin_cprim[] = {
	alloc_dummy,
	update_dummy,
	array_get_addr,
	array_get_float,
	array_get,
	array_set_addr,
	array_set_float,
	array_set,
	array_unsafe_get_float,
	array_unsafe_get,
	array_unsafe_set_addr,
	array_unsafe_set_float,
	array_unsafe_set,
	make_vect,
	make_array,
	compare,
	equal,
	notequal,
	lessthan,
	lessequal,
	greaterthan,
	greaterequal,
	output_value,
	output_value_to_string,
	output_value_to_buffer,
	format_float,
	float_of_string,
	int_of_float,
	float_of_int,
	neg_float,
	abs_float,
	add_float,
	sub_float,
	mul_float,
	div_float,
	exp_float,
	floor_float,
	fmod_float,
	frexp_float,
	ldexp_float,
	log_float,
	log10_float,
	modf_float,
	sqrt_float,
	power_float,
	sin_float,
	sinh_float,
	cos_float,
	cosh_float,
	tan_float,
	tanh_float,
	asin_float,
	acos_float,
	atan_float,
	atan2_float,
	ceil_float,
	eq_float,
	neq_float,
	le_float,
	lt_float,
	ge_float,
	gt_float,
	float_of_bytes,
	classify_float,
	gc_stat,
	gc_counters,
	gc_get,
	gc_set,
	gc_minor,
	gc_major,
	gc_full_major,
	gc_major_slice,
	gc_compaction,
	hash_univ_param,
	input_value,
	input_value_from_string,
	marshal_data_size,
	int_of_string,
	format_int,
	int32_neg,
	int32_add,
	int32_sub,
	int32_mul,
	int32_div,
	int32_mod,
	int32_and,
	int32_or,
	int32_xor,
	int32_shift_left,
	int32_shift_right,
	int32_shift_right_unsigned,
	int32_of_int,
	int32_to_int,
	int32_of_float,
	int32_to_float,
	int32_format,
	int32_of_string,
	int64_neg,
	int64_add,
	int64_sub,
	int64_mul,
	int64_div,
	int64_mod,
	int64_and,
	int64_or,
	int64_xor,
	int64_shift_left,
	int64_shift_right,
	int64_shift_right_unsigned,
	int64_of_int,
	int64_to_int,
	int64_of_float,
	int64_to_float,
	int64_of_int32,
	int64_to_int32,
	int64_of_nativeint,
	int64_to_nativeint,
	int64_format,
	int64_of_string,
	int64_bits_of_float,
	int64_float_of_bits,
	nativeint_neg,
	nativeint_add,
	nativeint_sub,
	nativeint_mul,
	nativeint_div,
	nativeint_mod,
	nativeint_and,
	nativeint_or,
	nativeint_xor,
	nativeint_shift_left,
	nativeint_shift_right,
	nativeint_shift_right_unsigned,
	nativeint_of_int,
	nativeint_to_int,
	nativeint_of_float,
	nativeint_to_float,
	nativeint_of_int32,
	nativeint_to_int32,
	nativeint_format,
	nativeint_of_string,
	caml_open_descriptor_in,
	caml_open_descriptor_out,
	caml_out_channels_list,
	channel_descriptor,
	caml_close_channel,
	caml_channel_size,
	caml_channel_size_64,
	caml_set_binary_mode,
	caml_flush_partial,
	caml_flush,
	caml_output_char,
	caml_output_int,
	caml_output_partial,
	caml_output,
	caml_seek_out,
	caml_seek_out_64,
	caml_pos_out,
	caml_pos_out_64,
	caml_input_char,
	caml_input_int,
	caml_input,
	caml_seek_in,
	caml_seek_in_64,
	caml_pos_in,
	caml_pos_in_64,
	caml_input_scan_line,
	lex_engine,
	md5_string,
	md5_chan,
	get_global_data,
	reify_bytecode,
	realloc_global,
	get_current_environment,
	invoke_traced_function,
	static_alloc,
	static_free,
	static_resize,
	obj_is_block,
	obj_tag,
	obj_set_tag,
	obj_block,
	obj_dup,
	obj_truncate,
	parse_engine,
	install_signal_handler,
	ml_string_length,
	create_string,
	string_get,
	string_set,
	string_equal,
	string_notequal,
	blit_string,
	fill_string,
	is_printable,
	bitvect_test,
	sys_exit,
	sys_open,
	sys_close,
	sys_file_exists,
	sys_remove,
	sys_rename,
	sys_chdir,
	sys_getcwd,
	sys_getenv,
	sys_get_argv,
	sys_system_command,
	sys_time,
	sys_random_seed,
	sys_get_config,
	terminfo_setup,
	terminfo_backup,
	terminfo_standout,
	terminfo_resume,
	register_named_value,
	weak_create,
	weak_set,
	weak_get,
	weak_get_copy,
	weak_check,
	final_register,
	ensure_stack_capacity,
	dynlink_open_lib,
	dynlink_close_lib,
	dynlink_lookup_symbol,
	dynlink_add_primitive,
	dynlink_get_current_libs,
	 0 };
char * names_of_builtin_cprim[] = {
	"alloc_dummy",
	"update_dummy",
	"array_get_addr",
	"array_get_float",
	"array_get",
	"array_set_addr",
	"array_set_float",
	"array_set",
	"array_unsafe_get_float",
	"array_unsafe_get",
	"array_unsafe_set_addr",
	"array_unsafe_set_float",
	"array_unsafe_set",
	"make_vect",
	"make_array",
	"compare",
	"equal",
	"notequal",
	"lessthan",
	"lessequal",
	"greaterthan",
	"greaterequal",
	"output_value",
	"output_value_to_string",
	"output_value_to_buffer",
	"format_float",
	"float_of_string",
	"int_of_float",
	"float_of_int",
	"neg_float",
	"abs_float",
	"add_float",
	"sub_float",
	"mul_float",
	"div_float",
	"exp_float",
	"floor_float",
	"fmod_float",
	"frexp_float",
	"ldexp_float",
	"log_float",
	"log10_float",
	"modf_float",
	"sqrt_float",
	"power_float",
	"sin_float",
	"sinh_float",
	"cos_float",
	"cosh_float",
	"tan_float",
	"tanh_float",
	"asin_float",
	"acos_float",
	"atan_float",
	"atan2_float",
	"ceil_float",
	"eq_float",
	"neq_float",
	"le_float",
	"lt_float",
	"ge_float",
	"gt_float",
	"float_of_bytes",
	"classify_float",
	"gc_stat",
	"gc_counters",
	"gc_get",
	"gc_set",
	"gc_minor",
	"gc_major",
	"gc_full_major",
	"gc_major_slice",
	"gc_compaction",
	"hash_univ_param",
	"input_value",
	"input_value_from_string",
	"marshal_data_size",
	"int_of_string",
	"format_int",
	"int32_neg",
	"int32_add",
	"int32_sub",
	"int32_mul",
	"int32_div",
	"int32_mod",
	"int32_and",
	"int32_or",
	"int32_xor",
	"int32_shift_left",
	"int32_shift_right",
	"int32_shift_right_unsigned",
	"int32_of_int",
	"int32_to_int",
	"int32_of_float",
	"int32_to_float",
	"int32_format",
	"int32_of_string",
	"int64_neg",
	"int64_add",
	"int64_sub",
	"int64_mul",
	"int64_div",
	"int64_mod",
	"int64_and",
	"int64_or",
	"int64_xor",
	"int64_shift_left",
	"int64_shift_right",
	"int64_shift_right_unsigned",
	"int64_of_int",
	"int64_to_int",
	"int64_of_float",
	"int64_to_float",
	"int64_of_int32",
	"int64_to_int32",
	"int64_of_nativeint",
	"int64_to_nativeint",
	"int64_format",
	"int64_of_string",
	"int64_bits_of_float",
	"int64_float_of_bits",
	"nativeint_neg",
	"nativeint_add",
	"nativeint_sub",
	"nativeint_mul",
	"nativeint_div",
	"nativeint_mod",
	"nativeint_and",
	"nativeint_or",
	"nativeint_xor",
	"nativeint_shift_left",
	"nativeint_shift_right",
	"nativeint_shift_right_unsigned",
	"nativeint_of_int",
	"nativeint_to_int",
	"nativeint_of_float",
	"nativeint_to_float",
	"nativeint_of_int32",
	"nativeint_to_int32",
	"nativeint_format",
	"nativeint_of_string",
	"caml_open_descriptor_in",
	"caml_open_descriptor_out",
	"caml_out_channels_list",
	"channel_descriptor",
	"caml_close_channel",
	"caml_channel_size",
	"caml_channel_size_64",
	"caml_set_binary_mode",
	"caml_flush_partial",
	"caml_flush",
	"caml_output_char",
	"caml_output_int",
	"caml_output_partial",
	"caml_output",
	"caml_seek_out",
	"caml_seek_out_64",
	"caml_pos_out",
	"caml_pos_out_64",
	"caml_input_char",
	"caml_input_int",
	"caml_input",
	"caml_seek_in",
	"caml_seek_in_64",
	"caml_pos_in",
	"caml_pos_in_64",
	"caml_input_scan_line",
	"lex_engine",
	"md5_string",
	"md5_chan",
	"get_global_data",
	"reify_bytecode",
	"realloc_global",
	"get_current_environment",
	"invoke_traced_function",
	"static_alloc",
	"static_free",
	"static_resize",
	"obj_is_block",
	"obj_tag",
	"obj_set_tag",
	"obj_block",
	"obj_dup",
	"obj_truncate",
	"parse_engine",
	"install_signal_handler",
	"ml_string_length",
	"create_string",
	"string_get",
	"string_set",
	"string_equal",
	"string_notequal",
	"blit_string",
	"fill_string",
	"is_printable",
	"bitvect_test",
	"sys_exit",
	"sys_open",
	"sys_close",
	"sys_file_exists",
	"sys_remove",
	"sys_rename",
	"sys_chdir",
	"sys_getcwd",
	"sys_getenv",
	"sys_get_argv",
	"sys_system_command",
	"sys_time",
	"sys_random_seed",
	"sys_get_config",
	"terminfo_setup",
	"terminfo_backup",
	"terminfo_standout",
	"terminfo_resume",
	"register_named_value",
	"weak_create",
	"weak_set",
	"weak_get",
	"weak_get_copy",
	"weak_check",
	"final_register",
	"ensure_stack_capacity",
	"dynlink_open_lib",
	"dynlink_close_lib",
	"dynlink_lookup_symbol",
	"dynlink_add_primitive",
	"dynlink_get_current_libs",
	 0 };
