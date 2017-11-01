#ifndef TINYJS_MATHFUNCTIONS_H
#define TINYJS_MATHFUNCTIONS_H

#include "../ClassLoader.h"


namespace JSM {

	class Math : public JSClass {
		//Math.abs(x) - returns absolute of given value
		static void abs(KoalaJS* js, BCVar *c, void *userdata);

		//Math.round(a) - returns nearest round of given value
		static void round(KoalaJS* js, BCVar *c, void *userdata);

		//Math.min(a,b) - returns minimum of two given values 
		static void min(KoalaJS* js, BCVar *c, void *userdata);

		//Math.max(a,b) - returns maximum of two given values  
		static void max(KoalaJS* js, BCVar *c, void *userdata);

		//Math.range(x,a,b) - returns value limited between two given values  
		static void range(KoalaJS* js, BCVar *c, void *userdata);

		//Math.sign(a) - returns sign of given value (-1==negative,0=zero,1=positive)
		static void sign(KoalaJS* js, BCVar *c, void *userdata);

		//Math.PI() - returns PI value
		static void PI(KoalaJS* js, BCVar *c, void *userdata);

		//Math.toDegrees(a) - returns degree value of a given angle in radians
		static void toDegrees(KoalaJS* js, BCVar *c, void *userdata);

		//Math.toRadians(a) - returns radians value of a given angle in degrees
		static void toRadians(KoalaJS* js, BCVar *c, void *userdata);

		//Math.sin(a) - returns trig. sine of given angle in radians
		static void sin(KoalaJS* js, BCVar *c, void *userdata);

		//Math.asin(a) - returns trig. arcsine of given angle in radians
		static void asin(KoalaJS* js, BCVar *c, void *userdata);

		//Math.cos(a) - returns trig. cosine of given angle in radians
		static void cos(KoalaJS* js, BCVar *c, void *userdata);

		//Math.acos(a) - returns trig. arccosine of given angle in radians
		static void acos(KoalaJS* js, BCVar *c, void *userdata);

		//Math.tan(a) - returns trig. tangent of given angle in radians
		static void tan(KoalaJS* js, BCVar *c, void *userdata);

		//Math.atan(a) - returns trig. arctangent of given angle in radians
		static void atan(KoalaJS* js, BCVar *c, void *userdata);

		//Math.sinh(a) - returns trig. hyperbolic sine of given angle in radians
		static void sinh(KoalaJS* js, BCVar *c, void *userdata);

		//Math.asinh(a) - returns trig. hyperbolic arcsine of given angle in radians
		static void asinh(KoalaJS* js, BCVar *c, void *userdata);

		//Math.cosh(a) - returns trig. hyperbolic cosine of given angle in radians
		static void cosh(KoalaJS* js, BCVar *c, void *userdata);

		//Math.acosh(a) - returns trig. hyperbolic arccosine of given angle in radians
		static void acosh(KoalaJS* js, BCVar *c, void *userdata);

		//Math.tanh(a) - returns trig. hyperbolic tangent of given angle in radians
		static void tanh(KoalaJS* js, BCVar *c, void *userdata);

		//Math.atan(a) - returns trig. hyperbolic arctangent of given angle in radians
		static void atanh(KoalaJS* js, BCVar *c, void *userdata);

		//Math.E() - returns E Neplero value
		static void E(KoalaJS* js, BCVar *c, void *userdata);

		//Math.log(a) - returns natural logaritm (base E) of given value
		static void log(KoalaJS* js, BCVar *c, void *userdata);

		//Math.log10(a) - returns logaritm(base 10) of given value
		static void log10(KoalaJS* js, BCVar *c, void *userdata);

		//Math.exp(a) - returns e raised to the power of a given number
		static void exp(KoalaJS* js, BCVar *c, void *userdata);

		//Math.pow(a,b) - returns the result of a number raised to a power (a)^(b)
		static void pow(KoalaJS* js, BCVar *c, void *userdata);

		//Math.sqr(a) - returns square of given value
		static void sqr(KoalaJS* js, BCVar *c, void *userdata);

		//Math.sqrt(a) - returns square root of given value
		static void sqrt(KoalaJS* js, BCVar *c, void *userdata);

		//Math.rand() - returns random double number
		static void rand(KoalaJS* js, BCVar *c, void *userdata);

		//Math.randInt(min, max) - returns random int number
		static void randInt(KoalaJS* js, BCVar *c, void *userdata);

		protected:
		void registerFunctions(KoalaJS *js, const std::string& className) {
			// --- Math and Trigonometry functions ---
			addFunction(js, className, "abs(a)", Math::abs, 0);
			addFunction(js, className, "round(a)", Math::round, 0);
			addFunction(js, className, "min(a,b)", Math::min, 0);
			addFunction(js, className, "max(a,b)", Math::max, 0);
			addFunction(js, className, "range(x,a,b)", Math::range, 0);
			addFunction(js, className, "sign(a)", Math::sign, 0);

			addFunction(js, className, "PI()", Math::PI, 0);
			addFunction(js, className, "toDegrees(a)", Math::toDegrees, 0);
			addFunction(js, className, "toRadians(a)", Math::toRadians, 0);
			addFunction(js, className, "sin(a)", Math::sin, 0);
			addFunction(js, className, "asin(a)", Math::asin, 0);
			addFunction(js, className, "cos(a)", Math::cos, 0);
			addFunction(js, className, "acos(a)", Math::acos, 0);
			addFunction(js, className, "tan(a)", Math::tan, 0);
			addFunction(js, className, "atan(a)", Math::atan, 0);
			addFunction(js, className, "sinh(a)", Math::sinh, 0);
			addFunction(js, className, "asinh(a)", Math::asinh, 0);
			addFunction(js, className, "cosh(a)", Math::cosh, 0);
			addFunction(js, className, "acosh(a)", Math::acosh, 0);
			addFunction(js, className, "tanh(a)", Math::tanh, 0);
			addFunction(js, className, "atanh(a)", Math::atanh, 0);

			addFunction(js, className, "E()", Math::E, 0);
			addFunction(js, className, "log(a)", Math::log, 0);
			addFunction(js, className, "log10(a)", Math::log10, 0);
			addFunction(js, className, "exp(a)", Math::exp, 0);
			addFunction(js, className, "pow(a,b)", Math::pow, 0);

			addFunction(js, className, "sqr(a)", Math::sqr, 0);
			addFunction(js, className, "sqrt(a)", Math::sqrt, 0);    
			addFunction(js, className, "rand()", Math::rand, 0);
			addFunction(js, className, "randInt(min, max)", Math::randInt, 0);    
		}

		public:
		DECL_INSTANCE(Math)
	};

};


#endif
