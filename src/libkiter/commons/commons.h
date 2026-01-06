/*
 * commons.h
 *
 *  Created on: 1 févr. 2011
 *      Author: toky
 *
 */

#ifndef COMMONS_H_
#define COMMONS_H_

#include <commons/verbose.h>
#include <commons/KiterRegistry.h>

#include <numeric>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iomanip>

#include <stack>
#include <iostream>
#include <cmath>
#include <string>
#include <cstring>
#include <limits>
#include <map>
#include <vector>
#include <cstdlib>
#include <commons/basic_types.h>
#include <array>


// Naively inspired from https://en.cppreference.com/w/cpp/language/operators
template <typename V>
class Fraction {
    V n, d;
public:
	Fraction(V n = 0, V d = 1) : n(n/std::gcd(n, d)), d(d/std::gcd(n, d)) { }
    V numerator() const { return n; }
    V denominator() const { return d; }

    // COMPARE WITH FRAC ==, < >

    bool operator>(const Fraction& rhs) const {
        return (not this->operator ==(rhs)) and (not this->operator <(rhs));
    }

    bool operator==(const Fraction& rhs) const {
        return this->numerator() == rhs.numerator() && this->denominator() == rhs.denominator();
    }


    // This operator from boost source.
    bool operator<(const Fraction& r) const {
    	// Avoid repeated construction
        const V  zero(0);

    	    // This should really be a class-wide invariant.  The reason for these
    	    // checks is that for 2's complement systems, INT_MIN has no corresponding
    	    // positive, so negating it during normalization keeps it INT_MIN, which
    	    // is bad for later calculations that assume a positive denominator.

        VERBOSE_ASSERT(this->d > zero, "Internal error.") ;
        VERBOSE_ASSERT(r.d > zero, "Internal error.") ;

    	    // Determine relative order by expanding each value to its simple continued
    	    // fraction representation using the Euclidian GCD algorithm.
    	    struct { V  n, d, q, r; }
    	     ts = { this->n, this->d, (this->n / this->d),
    	     (this->n % this->d) },
    	     rs = { r.n, r.d, static_cast<V>(r.n / r.d),
    	     (r.n % r.d) };
    	    unsigned  reverse = 0u;

    	    // Normalize negative moduli by repeatedly adding the (positive) denominator
    	    // and decrementing the quotient.  Later cycles should have all positive
    	    // values, so this only has to be done for the first cycle.  (The rules of
    	    // C++ require a nonnegative quotient & remainder for a nonnegative dividend
    	    // & positive divisor.)
    	    while ( ts.r < zero )  { ts.r += ts.d; --ts.q; }
    	    while ( rs.r < zero )  { rs.r += rs.d; --rs.q; }

    	    // Loop through and compare each variable's continued-fraction components
    	    for ( ;; )
    	    {
    	        // The quotients of the current cycle are the continued-fraction
    	        // components.  Comparing two c.f. is comparing their sequences,
    	        // stopping at the first difference.
    	        if ( ts.q != rs.q )
    	        {
    	            // Since reciprocation changes the relative order of two variables,
    	            // and c.f. use reciprocals, the less/greater-than test reverses
    	            // after each index.  (Start w/ non-reversed @ whole-number place.)
    	            return reverse ? ts.q > rs.q : ts.q < rs.q;
    	        }

    	        // Prepare the next cycle
    	        reverse ^= 1u;

    	        if ( (ts.r == zero) || (rs.r == zero) )
    	        {
    	            // At least one variable's c.f. expansion has ended
    	            break;
    	        }

    	        ts.n = ts.d;         ts.d = ts.r;
    	        ts.q = ts.n / ts.d;  ts.r = ts.n % ts.d;
    	        rs.n = rs.d;         rs.d = rs.r;
    	        rs.q = rs.n / rs.d;  rs.r = rs.n % rs.d;
    	    }

    	    // Compare infinity-valued components for otherwise equal sequences
    	    if ( ts.r == rs.r )
    	    {
    	        // Both remainders are zero, so the next (and subsequent) c.f.
    	        // components for both sequences are infinity.  Therefore, the sequences
    	        // and their corresponding values are equal.
    	        return false;
    	    }
    	    else
    	    {

    	        // Exactly one of the remainders is zero, so all following c.f.
    	        // components of that variable are infinity, while the other variable
    	        // has a finite next c.f. component.  So that other variable has the
    	        // lesser value (modulo the reversal flag!).
    	        return ( ts.r != zero ) != static_cast<bool>( reverse );
    	    }
    }


    // COMPARE WITH V ==, < >

    bool operator>(const V& rhs) const {
        return (not this->operator ==(rhs)) and (not this->operator <(rhs));
    }

    bool operator==(const V& i) const {
    	return (this->denominator() == 1) and (this->n == i);
    }
    bool operator<(const V& i) const {

       	// Avoid repeated construction
       	const V zero = 0;

       	// Break value into mixed-fraction form, w/ always-nonnegative remainder
        VERBOSE_ASSERT(this->d > zero, "always-nonnegative remainder");
       	V  q = this->n / this->d, r = this->n % this->d;
       	while(r < zero)  { r += this->d; --q; }

       	// Compare with just the quotient, since the remainder always bumps the
       	// value up.  [Since q = floor(n/d), and if n/d < i then q < i, if n/d == i
       	// then q == i, if n/d == i + r/d then q == i, and if n/d >= i + 1 then
       	// q >= i + 1 > i; therefore n/d < i iff q < i.]
       	return q < i;
       }

    // OPERATION WITH FRAC * /


    Fraction& operator*=(const Fraction& rhs) {
            V new_n = n * rhs.n/std::gcd(n * rhs.n, d * rhs.d);
            d = d * rhs.d/std::gcd(n * rhs.n, d * rhs.d);
            n = new_n;
            return *this;
    }


    // This operator from boost source.
    Fraction& operator/=(const Fraction& r) {
           // Avoid repeated construction
           const V  zero(0);

           // Protect against self-modification
               V r_num = r.n;
               V r_den = r.d;


               VERBOSE_ASSERT(r_num != zero, "Cannot divide by zero.") ;
               // Trap division by zero

               if (this->n == zero)
                   return *this;

               // Avoid overflow and preserve normalization
               V gcd1 = std::gcd(this->n, r_num);
               V gcd2 = std::gcd(r_den, this->d);
               this->n = (this->n/gcd1) * (r_den/gcd2);
               this->d = (this->d/gcd2) * (r_num/gcd1);

               if (this->d < zero) {
            	   this->n = -this->n;
            	   this->d = -this->d;
               }
               return *this;
        }


    // OPERATION WITH V /

    // This operator from boost source.
    Fraction& operator/=(const V& i) {
        // Avoid repeated construction
        const V  zero(0);

        VERBOSE_ASSERT(i != zero, "Cannot divide by zero.") ;
        if(this->n == zero) return *this;

        // Avoid overflow and preserve normalization
        V const gcd = std::gcd(this->n, i);
        this->n /= gcd;
        this->d *= i / gcd;

        if(this->d < zero) {
        	this->n = -this->n;
        	this->d = -this->d;
        }

        return *this;
     }






};

template<typename V>
std::ostream& operator<<(std::ostream& out, const Fraction<V>& f)
{
   return out << f.numerator() << '/' << f.denominator() ;
}


template<typename V>
bool operator<=(const Fraction<V>& lhs, const V& rhs)
{
    return not (lhs > rhs);
}

template<typename V>
bool operator>(const Fraction<V>& lhs, const V& rhs)
{
    return lhs > rhs;
}


template<typename V>
bool operator<=(const Fraction<V>& lhs, const Fraction<V>&  rhs)
{
    return not (lhs > rhs);
}



template<typename V>
bool operator!=(const Fraction<V>& lhs, const V& rhs)
{
    return !((lhs.numerator() == rhs) && (lhs.denominator() == 1));
}


template<typename V>
bool operator!=(const Fraction<V>& lhs, const Fraction<V>&  rhs)
{
    return !((lhs.numerator() == rhs.numerator()) && (lhs.denominator() == rhs.denominator()));
}


template<typename V>
Fraction<V> operator/(Fraction<V> lhs, const Fraction<V>& rhs)
{
    return lhs /= rhs;
}

template<typename V>
Fraction<V> operator*(Fraction<V> lhs, const Fraction<V>& rhs)
{
    return lhs *= rhs;
}

template<typename V>
Fraction<V> operator*(Fraction<V> lhs, const V& rhs)
{
    return lhs *= Fraction<V>(rhs);
}


typedef         Fraction<EXEC_COUNT> EXEC_COUNT_FRACT   ;
typedef    Fraction<TOKEN_UNIT>      TOKEN_FRACT        ;




namespace commons
{



 const std::string getFilename(const std::string s);



// en cours de reflexion ...
template<class Type>
	void deleteByIterating(Type IteratingContain) {
	for (Type it = IteratingContain->begin(); it != IteratingContain->end() ; it++) {delete (*it).second;};
}



// from http://stackoverflow.com/questions/4442658/c-parse-int-from-string
template<class T>
    T fromString(const std::string& s)
{
     std::istringstream stream (s);
     T t;
     stream >> t;
     return t;
}


template<typename T>
std::vector<T> split(const std::string &s, const char &delim) {
    std::vector<T> elems;
    std::string_view str_view(s);
    size_t start = 0;
    size_t end = str_view.find(delim);

    while (end != std::string_view::npos) {
        elems.push_back(commons::fromString<T>(std::string(str_view.substr(start, end - start))));
        start = end + 1;
        end = str_view.find(delim, start);
    }
    // Add the last segment
    elems.push_back(commons::fromString<T>(std::string(str_view.substr(start))));

    return elems;
}

//// This cannot be done easily without breaking calls
//template< typename T >
//std::vector<T> fromString(const std::string& s)
//{
//    return commons::split<T> (s, ',');
//}

template<>
char *fromString<char*>(const std::string& str);

template<>
const char *fromString<const char*>(const std::string& str);



template <typename T>
bool findInVector(const std::vector<T>& vect,  T v) {
    for (T ov : vect) {
        if (ov == v) return true;
    }
    return false;
}

template<typename T>
    std::string toString(const T& t)
{
     std::ostringstream stream;
     stream << t;
     return stream.str();
}

template<typename T, typename Q>
    std::string toString(const std::pair<T,Q>& v)
{
	return "<" + commons::toString<T>(std::get<0>(v)) +  "," +  commons::toString<Q>(std::get<1>(v))  + ">";
}

template<typename T>
    std::string toString(const std::vector<T>& t)
{

	 std::stringstream s;
	 s << "{";
	 bool first = true;
	 for (auto const & myt : t) {
		 if (!first) s << ",";
		 s << commons::toString(myt);
		 first = false;
	 }
	 s << "}";
	 return s.str();
}

template<typename T, typename Q>
    std::string toString(const std::map<T,Q>& v)
    {
        std::ostringstream stream;
        for (auto item : v) {
            stream << commons::toString<T>(item.first) << ":" << commons::toString<Q>(item.second) << " ";
        }
        return stream.str();
    }

    template<>
    std::string toString(const std::vector<std::string>& t);

    template<>
    std::string toString(const std::set<long  int, std::less<long  int>, std::allocator<long  int> >& t) ;
template<>
    std::string toString(const std::set<long unsigned int, std::less<long unsigned int>, std::allocator<long unsigned int> >& t) ;

template<>
std::string toString< std::vector < std::tuple<ARRAY_INDEX, ARRAY_INDEX, ARRAY_INDEX> > >(const std::vector < std::tuple<ARRAY_INDEX, ARRAY_INDEX, ARRAY_INDEX> >& v);

template<>
std::string toString< std::tuple<ARRAY_INDEX, ARRAY_INDEX, ARRAY_INDEX> >(const std::tuple<ARRAY_INDEX, ARRAY_INDEX, ARRAY_INDEX>& v);


template<>
std::string toString< std::pair<unsigned long , unsigned long> >(const std::pair<unsigned long , unsigned long>& v);
template<>
std::string toString< std::pair<TIME_UNIT , ARRAY_INDEX> >(const std::pair<TIME_UNIT , ARRAY_INDEX>& v);


template<>
std::string toString< std::vector<int> >(const std::vector<int>& v);
template<>
std::string toString< std::vector<ARRAY_INDEX> >(const std::vector<ARRAY_INDEX>& v);


template<>
std::string toString< std::vector<long double> >(const std::vector<long double>& v);

template<>
std::string toString< std::vector<unsigned int> >(const std::vector<unsigned int>& v);

// from http://stackoverflow.com/questions/1430757/c-array-to-string
template <class Str, class It>
Str join(It begin, const It end, const Str &sep)
{
  typedef typename Str::value_type     char_type;
  typedef typename Str::traits_type    traits_type;
  typedef typename Str::allocator_type allocator_type;
  typedef std::basic_ostringstream<char_type,traits_type,allocator_type>
                                       ostringstream_type;
  ostringstream_type result;

  if(begin!=end)
    result << commons::toString(*begin++);
  while(begin!=end) {
    result << sep;
    result << commons::toString(*begin++);
  }
  return result.str();
}

template <class T>
std::string  join(const std::vector<T>& vec, const std::string  &sep) {
    return join(vec.begin(),vec.end(), sep);
}

// // This should not be necessary
//
//    template<typename T>
//    std::string vectorAsStr(const std::vector<T>& t)
//    {
//        std::stringstream s;
//        for (typename std::vector<T>::size_type idx = 0 ; idx < t.size() ; idx++) {
//            if (idx > 0) {
//                s << ",";
//            }
//            s << commons::toString(t[idx]);
//        }
//        return s.str();
//    }
//
//    template<>
//    std::string vectorAsStr(const std::vector<TIME_UNIT>& t)
//    {
//        std::stringstream s;
//        for (typename std::vector<TIME_UNIT>::size_type idx = 0 ; idx < t.size() ; idx++) {
//            if (idx > 0) {
//                s << ",";
//            }
//            TIME_UNIT v = t[idx];
//            if ((std::floor(v)==std::ceil(v))) {
//                s << (unsigned long) (v) ;
//            } else {
//                s << commons::toString(v);
//            }
//        }
//        return s.str();
//    }


inline bool isInteger(TIME_UNIT v){
    TIME_UNIT intpart;
    TIME_UNIT remain = std::modf((TIME_UNIT)  v, (TIME_UNIT*) &intpart);

    return ( (TIME_UNIT) remain >= (TIME_UNIT) 0.0) && ((TIME_UNIT)  remain <= (TIME_UNIT)  0.0);
}




bool        is_readable (   const std::string&  file );

char *      string2char (   const std::string&  str);

// http://stackoverflow.com/questions/17333/most-effective-way-for-float-and-double-comparison
bool        AreSame     (   double a,
                            double b);
template <typename T>
inline bool AreSame(T a, T b) {
    return std::fabs((a - b)) < std::numeric_limits<T>::epsilon();
}

template<typename A,typename B>
 inline long double division(A a,B b) {return  (long double)a /(long double)b;}


 std::string getEnv(std::string name) ;

std::string latexize(std::string) ;


template <typename entier,typename flottant>
   inline entier ceil  ( flottant v , entier c){ return (entier) std::ceil ((flottant) v / (flottant) c) * c; }
template <typename entier,typename flottant>
   inline entier floor ( flottant v , entier c){ return (entier) std::floor((flottant) v / (flottant) c) * c; }
template <typename entier>
   inline entier ceil  ( entier v , entier c){ return (entier) std::ceil ((TIME_UNIT) v / (TIME_UNIT) c) * c; }
template <typename entier>
   inline const entier floor ( entier v , entier c){ return (entier) std::floor((TIME_UNIT) v / (TIME_UNIT) c) * c; }
template <typename entier>
   inline entier flooru  ( entier v , entier c){ return (entier) std::floor(commons::division(v,c)) * c; }
template <typename entier>
   inline entier ceilu  ( entier v , entier c){ return (entier) std::ceil(commons::division(v,c)) * c; }
template <typename entier>
   inline entier ceild  ( entier v , entier c){ if  ((v % c) == 0) {return v +c ;} else {return ceilu(v,c);};}

template <typename entier>
   inline entier floord  ( entier v , entier c){ if  ((v % c) == 0) {return v -c ;} else {return flooru(v,c);};}

inline std::string ConvertRGBtoHex(int num) {
         static std::string hexDigits = "0123456789ABCDEF";
         std::string rgb;
         for (int i=(3*2) - 1; i>=0; i--) {
             rgb += hexDigits[((num >> i*4) & 0xF)];
         }
         return rgb;
     }

inline std::string ConvertRGBtoHex(int r, int g, int b) {
      int rgbNum = ((r & 0xff) << 16)
          | ((g & 0xff) << 8)
          | (b & 0xff);

       return ConvertRGBtoHex(rgbNum);
  }
/**
 * Stupid way to do it, but easy to find ...*
 * other way welcome !
 */
TIME_UNIT roundIt(TIME_UNIT val,TIME_UNIT p);
//

std::vector<std::string> splitSDF3List(const std::string &s);

int fibo (int index);
 
const std::string runCmd( const std::string sendme , TIME_UNIT timeout) ;





template <typename entier>
inline entier modulo(entier a, entier b) {
  const entier result = a % b;
  return result >= 0 ? result : result + b;
}



template <typename entier>
std::pair<entier,entier> extended_euclide (entier _a, entier _b, entier _c) {

	// Kuṭṭaka, Aryabhata's algorithm for solving linear Diophantine equations in two unknowns


	entier gcdab = std::gcd(_a,_b) ;

	VERBOSE_ASSERT (_c % gcdab == 0, "No solution");

	entier a = _a/ gcdab;
	entier b = _b/ gcdab;
	entier c = _c/ gcdab;

	std::pair<entier,entier> r (a,b);
	std::pair<entier,entier> s (1,0);
	std::pair<entier,entier> t (0,1);

    while (r.second != 0) {
    	entier quotient = r.first / r.second;
    	r = std::pair<entier,entier> ( r.second ,  r.first - quotient * r.second );
    	s = std::pair<entier,entier> ( s.second ,  s.first - quotient * s.second );
    	t = std::pair<entier,entier> ( t.second ,  t.first - quotient * t.second );
    }

    VERBOSE_INFO ( "A:" <<  _a << ", B:" <<  _b << ", C:" <<  _c );
    VERBOSE_INFO ( "gcdab:" <<  gcdab );
    VERBOSE_INFO ( "A:" <<  a << ", B:" <<  b << ", C:" <<  c );
    VERBOSE_INFO ( "Bézout coefficients:" <<  s.first << "," <<  t.first );
    VERBOSE_INFO ( "greatest common divisor:" <<  r.first);
    VERBOSE_INFO ( "quotients by the gcd:" <<  t.second<< "," <<   s.second);

    return std::pair<entier,entier>  ( (c * s.first), (c * t.first));

}



template <typename VALUE_TYPE>
VALUE_TYPE get_parameter ( parameters_list_t params , std::string name , VALUE_TYPE default_value) {

	VALUE_TYPE value = default_value ;

	 if (params.find(name)!= params.end() ) {
		 value = commons::fromString<VALUE_TYPE>(params[name]);
		 VERBOSE_INFO(name << " was to " << value);
	 } else {
		 VERBOSE_WARNING(name << " parameter is not found.");
	 }

	 return value;
}



    std::vector<std::string> parseCsvLine(const std::string& line) ;



}// end of commons namespace

#endif /* COMMONS_H_ */
