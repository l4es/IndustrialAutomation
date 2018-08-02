#include "Common.h"
#include ".\equation.h"
#include "VariableMenager.h"

using namespace boost::spirit;

namespace Calculation
{
	namespace calc_funcs
	{
		//////////////////////////////////////////////////////////////////////////
		// Unary funcs
		//////////////////////////////////////////////////////////////////////////
		//Do Op
		template <typename op>
		struct do_unary_op
		{
			do_unary_op(op const& the_op, CEquation::TStack& eval_)
				: m_op(the_op), eval(eval_) {}

				void operator()(const wchar_t*, const wchar_t*) const
				{
					typedef CEquation::TStack::value_type t;
					t val = eval.top();

					eval.pop();
					val = m_op(val);
					eval.push(val);
				}
				op m_op;
				CEquation::TStack& eval;
		};
		//Make Op
		template <class op>
			do_unary_op<op> make_unary_op(op const& the_op, CEquation::TStack& eval)
		{
			return do_unary_op<op>(the_op, eval);
		};

		//////////////////////////////////////////////////////////////////////////
		//Negative
		template<typename T>
		struct do_neg
			:unary_function<T, T>
		{
			T operator()(T val) const {return -val;};
		};

		//////////////////////////////////////////////////////////////////////////
		//Sin
		template<typename T>
		struct do_fn_sin
			:unary_function<T const&, T>
		{
			T operator()(T const& val) const{return sin(val);}
		};

		//////////////////////////////////////////////////////////////////////////
		//Cos
		template<typename T>
		struct do_fn_cos
			:unary_function<T const&, T>
		{
			T operator()(T const& val) const{return cos(val);}
		};

		//////////////////////////////////////////////////////////////////////////
		//Inverse
		template<typename T>
		struct do_fn_inv
			:unary_function<T const&, T>
		{
			T operator()(T const& val) const{return (T)(!static_cast<ULONG>(val));}
		};

		//////////////////////////////////////////////////////////////////////////
		//Binary Inverse
		template<typename T>
		struct do_fn_binv
			:unary_function<T const&, T>
		{
			T operator()(T const& val) const
			{
				ULONG tmp = ~(static_cast<ULONG>(val));
				return (T)(tmp);
			}
		};

		//////////////////////////////////////////////////////////////////////////
		//Abs
		template<typename T>
		struct do_fn_abs
			:unary_function<T const&, T>
		{
			T operator()(T const& val) const{return abs(val);}
		};

		//////////////////////////////////////////////////////////////////////////
		//Exp
		template<typename T>
		struct do_fn_exp
			:unary_function<T const&, T>
		{
			T operator()(T const& val) const{return exp(val);}
		};

		//////////////////////////////////////////////////////////////////////////
		//ArcTan
		template<typename T>
		struct do_fn_atan
			:unary_function<T const&, T>
		{
			T operator()(T const& val) const{return atan(val);}
		};

		//////////////////////////////////////////////////////////////////////////
		//ArcCos
		template<typename T>
		struct do_fn_acos
			:unary_function<T const&, T>
		{
			T operator()(T const& val) const{return acos(val);}
		};

		//////////////////////////////////////////////////////////////////////////
		//Ln
		template<typename T>
		struct do_fn_ln
			:unary_function<T const&, T>
		{
			T operator()(T const& val) const {return log(val)/log(2.718282);}
		};

		//////////////////////////////////////////////////////////////////////////
		//Sqr
		template<typename T>
		struct do_fn_sqr
			:unary_function<T const&, T>
		{
			T operator()(T const& val) const {return pow(val,2);}
		};

		//////////////////////////////////////////////////////////////////////////
		//Sqrt
		template<typename T>
		struct do_fn_sqrt
			:unary_function<T const&, T>
		{
			T operator()(T const& val) const {return sqrt(val);}
		};


		//////////////////////////////////////////////////////////////////////////
		// Binary funcs
		//////////////////////////////////////////////////////////////////////////
		//Do Op
		template <typename op>
		struct do_binary_op
		{
			do_binary_op(op const& the_op, CEquation::TStack& eval_): m_op(the_op), eval(eval_){}

			void operator()(const wchar_t*, const wchar_t*) const
			{
				typedef CEquation::TStack::value_type t;
				if(eval.empty()) return;
				t rhs = eval.top();
				eval.pop();
				if(eval.empty()) return;
				t lhs = eval.top();
				eval.pop();
				eval.push(m_op(lhs, rhs));
			}

			op m_op;
			CEquation::TStack& eval;
		};

		//Make Op
		template <class op>
			do_binary_op<op> make_binary_op(op const& the_op, CEquation::TStack& eval)
		{
			return do_binary_op<op>(the_op, eval);
		};

		//////////////////////////////////////////////////////////////////////////
		//AND
		template<typename T>
		struct do_fn_and
			:binary_function<T, T, T>
		{
			T operator()(const T& lhs, const T& rhs) const
			{
				ULONG tmp = static_cast<ULONG>(lhs) & static_cast<ULONG>(rhs);
				return tmp;
			}
		};

		//////////////////////////////////////////////////////////////////////////
		//OR
		template<typename T>
		struct do_fn_or
			:binary_function<T, T, T>
		{
			T operator()(const T& lhs, const T& rhs) const
			{
				ULONG tmp = static_cast<ULONG>(lhs) | static_cast<ULONG>(rhs);
				return tmp;
			}
		};

		//////////////////////////////////////////////////////////////////////////
		//Modulus
		template<typename T>
		struct do_fn_modulus
			:binary_function<T, T, T>
		{
			T operator()(const T& lhs, const T& rhs) const
			{
				ULONG tmp = static_cast<ULONG>(lhs) % static_cast<ULONG>(rhs);
				return tmp;
			}
		};

		//////////////////////////////////////////////////////////////////////////
		//Left Shift
		template<typename T>
		struct do_fn_lshift
			:binary_function<T, T, T>
		{
			T operator()(const T& lhs, const T& rhs) const
			{
				ULONG tmp = static_cast<ULONG>(lhs) << static_cast<ULONG>(rhs);
				return tmp;
			}
		};

		//////////////////////////////////////////////////////////////////////////
		//Right Shift
		template<typename T>
		struct do_fn_rshift
			:binary_function<T, T, T>
		{
			T operator()(const T& lhs, const T& rhs) const
			{
				ULONG tmp = static_cast<ULONG>(lhs) >> static_cast<ULONG>(rhs);
				return tmp;
			}
		};

		//////////////////////////////////////////////////////////////////////////
		//Less (<)
		template<typename T>
		struct do_fn_less
			:binary_function<T, T, T>
		{
			T operator()(const T& lhs, const T& rhs) const
			{
				return lhs < rhs;
			}
		};

		//////////////////////////////////////////////////////////////////////////
		//Less or Equal (<=)
		template<typename T>
		struct do_fn_lessorequal
			:binary_function<T, T, T>
		{
			T operator()(const T& lhs, const T& rhs) const
			{
				return lhs <= rhs;
			}
		};

		//////////////////////////////////////////////////////////////////////////
		//Equality (==)
		template<typename T>
		struct do_fn_equality
			:binary_function<T, T, T>
		{
			T operator()(const T& lhs, const T& rhs) const
			{
				return lhs == rhs;
			}
		};

		//////////////////////////////////////////////////////////////////////////
		//Not Equal (!=)
		template<typename T>
		struct do_fn_notequal
			:binary_function<T, T, T>
		{
			T operator()(const T& lhs, const T& rhs) const
			{
				return lhs != rhs;
			}
		};

		//////////////////////////////////////////////////////////////////////////
		//More (>)
		template<typename T>
		struct do_fn_more
			:binary_function<T, T, T>
		{
			T operator()(const T& lhs, const T& rhs) const
			{
				return lhs > rhs;
			}
		};

		//////////////////////////////////////////////////////////////////////////
		//More or Equal (>=)
		template<typename T>
		struct do_fn_moreorequal
			:binary_function<T, T, T>
		{
			T operator()(const T& lhs, const T& rhs) const
			{
				return lhs >= rhs;
			}
		};

		//////////////////////////////////////////////////////////////////////////
		//Variable
		struct do_fn_variable
		{
			do_fn_variable(CEquation::TStack& stack_)
				: stack(stack_){}

				template <typename IteratorT>
					void operator()(IteratorT first, IteratorT last) const
				{
					wstring variable(first,last);
					CVariableMenagerPtr mngr;
					
					CComVariant v = mngr->GetVariable(variable);
					//Делаем boolean не 0/-1, а 0/1
					if(v.vt == VT_BOOL)
						v = v.boolVal?1:0;
					v.ChangeType(VT_R8);
					stack.push(v.dblVal);
				}

				CEquation::TStack& stack;
		};

		//////////////////////////////////////////////////////////////////////////
		// Push value into stack (constants)
		//////////////////////////////////////////////////////////////////////////
		struct push_val
		{
			push_val(CEquation::TStack& stack)
				: _stack(stack){}

				void operator()(double val) const
				{
					_stack.push(val);
				}

				CEquation::TStack& _stack;
		};
		//////////////////////////////////////////////////////////////////////////
	}

	//Grammar
	struct calc_grammar
		: grammar<calc_grammar>
	{
		calc_grammar(CEquation::TStack& s):_stack(s){};
		CEquation::TStack& _stack;

		template <typename ScannerT>
		struct definition
		{
			rule<ScannerT>	function,
							factor,
							term,
							expression,
							equation;
			rule<ScannerT> const& start() const {return equation;};

			definition(calc_grammar const& self)
			{
				using namespace calc_funcs;

				//Общая структура
				// equation		= <expression>	{oper} [expression]...
				// expression	= <term>		{oper} [term]...
				// term			= <factor>		{oper} [factor]...
				// factor		= [variable]|[constant]|[function]|[equation]
				// function		= [func1]|[func2]...

				//Описываем структуру булевых выражений
				equation = expression
					>> *((L'<' >> expression)			[make_binary_op(do_fn_less<double>(), self._stack)]
					|	(as_lower_d[L"<="] >> expression)[make_binary_op(do_fn_lessorequal<double>(), self._stack)]
					|	(as_lower_d[L"=="] >> expression)[make_binary_op(do_fn_equality<double>(), self._stack)]
					|	(as_lower_d[L"!="] >> expression)[make_binary_op(do_fn_notequal<double>(), self._stack)]
					|	(L'>' >> expression)				[make_binary_op(do_fn_more<double>(), self._stack)]
					|	(as_lower_d[L">="] >> expression)[make_binary_op(do_fn_moreorequal<double>(), self._stack)]
					);

				//Описываем структуру выражений
				expression = term
					>> *(   (L'+' >> term)			[make_binary_op(plus<double>(), self._stack)]
					|   (L'-' >> term)				[make_binary_op(minus<double>(), self._stack)]
					|   (L'|' >> term)				[make_binary_op(do_fn_or<double>(), self._stack)]
					|   (as_lower_d[L"or"] >> term)	[make_binary_op(do_fn_or<double>(), self._stack)]
					);

				//Описываем структуру терма (множитель)
				term = factor
					>> *(	(L'*' >> factor)				[make_binary_op(multiplies<double>(), self._stack)]
					|   (L'/' >> factor)					[make_binary_op(divides<double>(), self._stack)]
					|   (L'&' >> factor)					[make_binary_op(do_fn_and<double>(), self._stack)]
					|   (as_lower_d[L"and"] >> factor)	[make_binary_op(do_fn_and<double>(), self._stack)]
					|   (L'%' >> factor)					[make_binary_op(do_fn_modulus<double>(), self._stack)]
					|   (as_lower_d[L"mod"] >> factor)	[make_binary_op(do_fn_modulus<double>(), self._stack)]
					|   (as_lower_d[L"<<"] >> factor)	[make_binary_op(do_fn_lshift<double>(), self._stack)]
					|   (as_lower_d[L">>"] >> factor)	[make_binary_op(do_fn_rshift<double>(), self._stack)]
					);

				//Описываем функции
				function = 
					(L'-' >> factor)[make_unary_op(do_neg<double>(), self._stack)]
					|   (L'+' >> factor)

					|	(as_lower_d[L"sin"]>>L'(' >> expression >> L')')		[make_unary_op(do_fn_sin<double>(), self._stack)]
					|	(as_lower_d[L"cos"]>>L'(' >> expression >> L')')		[make_unary_op(do_fn_cos<double>(), self._stack)]
					|	(as_lower_d[L"abs"]>>L'(' >> expression >> L')')		[make_unary_op(do_fn_abs<double>(), self._stack)]
					|	(as_lower_d[L"exp"]>>L'(' >> expression >> L')')		[make_unary_op(do_fn_exp<double>(), self._stack)]
					|	(as_lower_d[L"arctan"]>>L'(' >> expression >> L')')	[make_unary_op(do_fn_atan<double>(), self._stack)]
					|	(as_lower_d[L"ln"]>>L'(' >> expression >> L')')		[make_unary_op(do_fn_ln<double>(), self._stack)]
					|	(as_lower_d[L"sqr"]>>L'(' >> expression >> L')')		[make_unary_op(do_fn_sqr<double>(), self._stack)]
					|	(as_lower_d[L"sqrt"]>>L'(' >> expression >> L')')		[make_unary_op(do_fn_sqrt<double>(), self._stack)]

					|	(L'~' >> factor)					[make_unary_op(do_fn_binv<double>(), self._stack)]
					|	(as_lower_d[L"bnot"] >> factor)	[make_unary_op(do_fn_binv<double>(), self._stack)]
					|	(L'!' >> factor)					[make_unary_op(do_fn_inv<double>(), self._stack)]
					|	(as_lower_d[L"not"] >> factor)	[make_unary_op(do_fn_inv<double>(), self._stack)]
					;

					//Описываем структуру множителя
					factor =
							real_p[push_val(self._stack)]
						|	lexeme_d[((alpha_p | range_p(L'а',L'я') | range_p(L'А',L'Я') | L'_' | L'$') >> *(alnum_p | alpha_p | range_p(L'а',L'я') | range_p(L'А',L'Я') | L'_' ))][do_fn_variable(self._stack)]
//						|	lexeme_d[((anychar_p | '_' | '$') >> *(alnum_p | anychar_p | '_' ))][do_fn_variable(self._stack)]
//						|	lexeme_d[(alpha_p | '_' | '$') >> *(alpha_p | alnum_p | '_')][do_fn_variable(self._stack)]
						|	function
						|   L'(' >> equation >> L')'
						;
			};
		};
	};
}

bool CEquation::CalcBool(wstring Expression)
{
	TStack stack;
	Calculation::calc_grammar grammar(stack);

	if(parse(Expression.c_str(), grammar, space_p).full)
	{
		double res = stack.top();
		stack.pop();
		return res?true:false;
	}
	else
	{
		ATLTRACE(L"Ошибка в выражении\n");
		return false;
	}
}

double CEquation::CalcDouble(wstring Expression)
{
	TStack stack;
	Calculation::calc_grammar grammar(stack);

	if(parse(Expression.c_str(), grammar, space_p).full)
	{
		double res = stack.top();
		stack.pop();
		return res;
	}
	else
	{
		ATLTRACE(L"Ошибка в выражении\n");
		return 0;
	}
}

int CEquation::CalcInt(wstring Expression)
{
	TStack stack;
	Calculation::calc_grammar grammar(stack);

	if(parse(Expression.c_str(), grammar, space_p).full)
	{
		double res = stack.top();
		stack.pop();
		return (int)res;
	}
	else
	{
		ATLTRACE(L"Ошибка в выражении\n");
		return 0;
	}
}