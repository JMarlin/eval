typedef literal int;
typedef literal (*lo_op)(literal input);
typedef literal (*lr_op)(literal left, literal right);
typedef token char*;

token readT() {} //fuck i dunno

literal eval(token instance) {

	lo_op left_function;
	lr_op lr_function;
	literal left_val, right_val;

	switch(matchT(instance)) {
		case LITERAL:
			return litval(instance);
			break;
		case LEFT_ONLY:
			if(!(left_function = leftop(instance)))
				return 0; //For now, unregognized tokens will just eval to 0.
			if(!(left_val = readT()))
				return 0;
			return (*left_function)(eval(left_val));
			break;
		case LEFT_RIGHT:
	 	 	if(!(lr_function = lrop(instance)))
				return 0; //For now, unregognized tokens will just eval to 0.
			if(!(left_val = readT()))
				return 0;
			if(!(right_val = readT()))
				return 0;
			return (*lr_function)(eval(left_val), eval(right_val));
			break;
	}

}

int main(int argc, char* argv[]) {

		show(eval(readT()));
		return 0;
		
}