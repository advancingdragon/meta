#include "support.h"

void meta_arg(void)
{
	do {
		read_literal("$");
		if (test_flag) {
			emit("emit_token();");
			emit_nl();
		}

		if (test_flag) { break; }

		read_string();
		if (test_flag) {
			emit("emit(");
			emit_token();
			emit(");");
			emit_nl();
		}
	} while (0);
}

void meta_output(void)
{
	do {
		read_literal("{");
		if (test_flag) {
			do {
				meta_arg();
			} while (test_flag);
			test_flag = 1;
			error_if_false();

			read_literal("}");
			error_if_false();

			emit("emit_nl();");
			emit_nl();
		}
	} while (0);
}

void meta_exp3(void)
{
	do {
		read_id();
		if (test_flag) {
			emit("meta_");
			emit_token();
			emit("();");
			emit_nl();
		}

		if (test_flag) { break; }

		read_string();
		if (test_flag) {
			emit("read_literal(");
			emit_token();
			emit(");");
			emit_nl();
		}

		if (test_flag) { break; }

		read_literal(".id");
		if (test_flag) {
			emit("read_id();");
			emit_nl();
		}

		if (test_flag) { break; }

		read_literal(".number");
		if (test_flag) {
			emit("read_number();");
			emit_nl();
		}

		if (test_flag) { break; }

		read_literal(".string");
		if (test_flag) {
			emit("read_string();");
			emit_nl();
		}

		if (test_flag) { break; }

		read_literal("(");
		if (test_flag) {
			meta_exp1();
			error_if_false();

			read_literal(")");
			error_if_false();
		}

		if (test_flag) { break; }

		read_literal(".e");
		if (test_flag) {
			emit("test_flag = 1;");
			emit_nl();
		}

		if (test_flag) { break; }

		read_literal("*");
		if (test_flag) {
			emit("do {");
			emit_nl();

			meta_exp3();
			error_if_false();

			emit("} while (test_flag);");
			emit_nl();

			emit("test_flag = 1;");
			emit_nl();
		}
	} while (0);
}

void meta_exp2(void)
{
	do {
		// open
		do {
			meta_exp3();
			if (test_flag) {
				emit("if (test_flag) {");
				emit_nl();
			}

			if (test_flag) { break; }

			meta_output();
			if (test_flag) {
				emit("if (1) {");
				emit_nl();
			}
		} while (0);
		// close

		if (test_flag) {
			do {
				// open
				do {
					meta_exp3();
					if (test_flag) {
						emit("error_if_false();");
						emit_nl();
					}

					if (test_flag) { break; }

					meta_output();
					if (test_flag) {
					}
				} while (0);
				// close

			} while (test_flag);
			test_flag = 1;
			error_if_false();

			emit("}");
			emit_nl();
		}
	} while (0);
}

void meta_exp1(void)
{
	do {
		emit("do {");
		emit_nl();
		if (1) {
			meta_exp2();
			error_if_false();

			do {
				// open
				do {
					read_literal("|");
					if (test_flag) {
						emit("if (test_flag) { break; }");
						emit_nl();

						meta_exp2();
						error_if_false();
					}
				} while (0);
				// close

			} while (test_flag);
			test_flag = 1;
			error_if_false();

			emit("} while (0);");
			emit_nl();
		}
	} while (0);
}

void meta_stat(void)
{
	do {
		read_id();
		if (test_flag) {
			emit("void meta_");
			emit_token();
			emit("(void)");
			emit_nl();

			emit("{");
			emit_nl();

			read_literal("=");
			error_if_false();

			meta_exp1();
			error_if_false();

			read_literal(";");
			error_if_false();

			emit("}");
			emit_nl();
		}
	} while (0);
}

void meta_program(void)
{
	do {
		read_literal(".syntax");
		if (test_flag) {
			read_id();
			error_if_false();

			emit("#include \"support.h\"");
			emit_nl();

			do {
				meta_stat();
			} while (test_flag);
			test_flag = 1;
			error_if_false();

			read_literal(".end");
			error_if_false();
		}
	} while (0);
}

// end
