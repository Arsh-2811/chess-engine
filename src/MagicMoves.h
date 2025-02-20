#ifndef _magicmovesh
#define _magicmovesh

#include <cstdint>

#define U64 std::uint64_t

namespace MagicMoves
{
	/*********MODIFY THE FOLLOWING IF NECESSARY********/
	//the default configuration is the best

	//Uncommont either one of the following or none
	// #define MINIMIZE_MAGIC
	//#define PERFECT_MAGIC_HASH unsigned short

	//the following works only for perfect magic hash or no defenitions above
	//it uses variable shift for each square
	//#define VARIABLE_SHIFT

	#define USE_INLINING /*the MMINLINE keyword is assumed to be available*/

	#ifndef __64_BIT_INTEGER_DEFINED__
		#define __64_BIT_INTEGER_DEFINED__
		#if defined(_MSC_VER) && _MSC_VER<1300
			typedef unsigned __int64 U64; //For the old microsoft compilers
		#else
			//typedef unsigned long long  U64; //Supported by MSC 13.00+ and C99
		#endif //defined(_MSC_VER) && _MSC_VER<1300
	#endif //__64_BIT_INTEGER_DEFINED__
	/***********MODIFY THE ABOVE IF NECESSARY**********/

	/*Defining the inlining keyword*/
	#ifdef USE_INLINING
		#ifdef _MSC_VER
			#define MMINLINE __forceinline
		#elif defined(__GNUC__)
			#define MMINLINE __inline__ __attribute__((always_inline))
		#else
			#define MMINLINE inline
		#endif
	#endif

	#ifndef C64
		#if (!defined(_MSC_VER) || _MSC_VER>1300)
			#define C64(constantU64) constantU64##ULL
		#else
			#define C64(constantU64) constantU64
		#endif
	#endif

	extern const U64 magicmoves_r_magics[64];
	extern const U64 magicmoves_r_mask[64];
	extern const U64 magicmoves_b_magics[64];
	extern const U64 magicmoves_b_mask[64];
	extern const unsigned int magicmoves_b_shift[64];
	extern const unsigned int magicmoves_r_shift[64];

	#ifndef VARIABLE_SHIFT
		#define MINIMAL_B_BITS_SHIFT(square) 55
		#define MINIMAL_R_BITS_SHIFT(square) 52
	#else
		#define MINIMAL_B_BITS_SHIFT(square) magicmoves_b_shift[square]
		#define MINIMAL_R_BITS_SHIFT(square) magicmoves_r_shift[square]
	#endif

	#ifndef PERFECT_MAGIC_HASH
		#ifdef MINIMIZE_MAGIC

			#ifndef USE_INLINING
				#define Bmagic(square, occupancy) *(magicmoves_b_indices[square]+((((occupancy)&magicmoves_b_mask[square])*magicmoves_b_magics[square])>>magicmoves_b_shift[square]))
				#define Rmagic(square, occupancy) *(magicmoves_r_indices[square]+((((occupancy)&magicmoves_r_mask[square])*magicmoves_r_magics[square])>>magicmoves_r_shift[square]))
				#define BmagicNOMASK(square, occupancy) *(magicmoves_b_indices[square]+(((occupancy)*magicmoves_b_magics[square])>>magicmoves_b_shift[square]))
				#define RmagicNOMASK(square, occupancy) *(magicmoves_r_indices[square]+(((occupancy)*magicmoves_r_magics[square])>>magicmoves_r_shift[square]))
			#endif //USE_INLINING

			//extern U64 magicmovesbdb[5248];
			extern const U64* magicmoves_b_indices[64];

			//extern U64 magicmovesrdb[102400];
			extern const U64* magicmoves_r_indices[64];

		#else //Don't Minimize database size

			#ifndef USE_INLINING
				#define Bmagic(square, occupancy) magicmovesbdb[square][(((occupancy)&magicmoves_b_mask[square])*magicmoves_b_magics[square])>>MINIMAL_B_BITS_SHIFT(square)]
				#define Rmagic(square, occupancy) magicmovesrdb[square][(((occupancy)&magicmoves_r_mask[square])*magicmoves_r_magics[square])>>MINIMAL_R_BITS_SHIFT(square)]
				#define BmagicNOMASK(square, occupancy) magicmovesbdb[square][((occupancy)*magicmoves_b_magics[square])>>MINIMAL_B_BITS_SHIFT(square)]
				#define RmagicNOMASK(square, occupancy) magicmovesrdb[square][((occupancy)*magicmoves_r_magics[square])>>MINIMAL_R_BITS_SHIFT(square)]
			#endif //USE_INLINING

			extern U64 magicmovesbdb[64][1<<9];
			extern U64 magicmovesrdb[64][1<<12];

		#endif //MINIMIAZE_MAGICMOVES
	#else //PERFCT_MAGIC_HASH defined
		#ifndef MINIMIZE_MAGIC

			#ifndef USE_INLINING
				#define Bmagic(square, occupancy) magicmovesbdb[magicmoves_b_indices[square][(((occupancy)&magicmoves_b_mask[square])*magicmoves_b_magics[square])>>MINIMAL_B_BITS_SHIFT(square)]]
				#define Rmagic(square, occupancy) magicmovesrdb[magicmoves_r_indices[square][(((occupancy)&magicmoves_r_mask[square])*magicmoves_r_magics[square])>>MINIMAL_R_BITS_SHIFT(square)]]
				#define BmagicNOMASK(square, occupancy) magicmovesbdb[magicmoves_b_indices[square][((occupancy)*magicmoves_b_magics[square])>>MINIMAL_B_BITS_SHIFT(square)]]
				#define RmagicNOMASK(square, occupancy) magicmovesrdb[magicmoves_r_indices[square][((occupancy)*magicmoves_r_magics[square])>>MINIMAL_R_BITS_SHIFT(square)]]
			#endif //USE_INLINING

			extern U64 magicmovesbdb[1428];
			extern U64 magicmovesrdb[4900];
			extern PERFECT_MAGIC_HASH magicmoves_b_indices[64][1<<9];
			extern PERFECT_MAGIC_HASH magicmoves_r_indices[64][1<<12];
		#else
			#error magicmoves - MINIMIZED_MAGIC and PERFECT_MAGIC_HASH cannot be used together
		#endif
	#endif //PERFCT_MAGIC_HASH

	#ifdef USE_INLINING
		static MMINLINE U64 Bmagic(const unsigned int square,const U64 occupancy)
		{
			#ifndef PERFECT_MAGIC_HASH
				#ifdef MINIMIZE_MAGIC
					return *(magicmoves_b_indices[square]+(((occupancy&magicmoves_b_mask[square])*magicmoves_b_magics[square])>>magicmoves_b_shift[square]));
				#else
					return magicmovesbdb[square][(((occupancy)&magicmoves_b_mask[square])*magicmoves_b_magics[square])>>MINIMAL_B_BITS_SHIFT(square)];
				#endif
			#else
				return magicmovesbdb[magicmoves_b_indices[square][(((occupancy)&magicmoves_b_mask[square])*magicmoves_b_magics[square])>>MINIMAL_B_BITS_SHIFT(square)]];
			#endif
		}
		static MMINLINE U64 Rmagic(const unsigned int square,const U64 occupancy)
		{
			#ifndef PERFECT_MAGIC_HASH
				#ifdef MINIMIZE_MAGIC
					return *(magicmoves_r_indices[square]+(((occupancy&magicmoves_r_mask[square])*magicmoves_r_magics[square])>>magicmoves_r_shift[square]));
				#else
					return magicmovesrdb[square][(((occupancy)&magicmoves_r_mask[square])*magicmoves_r_magics[square])>>MINIMAL_R_BITS_SHIFT(square)];
				#endif
			#else
				return magicmovesrdb[magicmoves_r_indices[square][(((occupancy)&magicmoves_r_mask[square])*magicmoves_r_magics[square])>>MINIMAL_R_BITS_SHIFT(square)]];
			#endif
		}
		static MMINLINE U64 BmagicNOMASK(const unsigned int square,const U64 occupancy)
		{
			#ifndef PERFECT_MAGIC_HASH
				#ifdef MINIMIZE_MAGIC
					return *(magicmoves_b_indices[square]+(((occupancy)*magicmoves_b_magics[square])>>magicmoves_b_shift[square]));
				#else
					return magicmovesbdb[square][((occupancy)*magicmoves_b_magics[square])>>MINIMAL_B_BITS_SHIFT(square)];
				#endif
			#else
				return magicmovesbdb[magicmoves_b_indices[square][((occupancy)*magicmoves_b_magics[square])>>MINIMAL_B_BITS_SHIFT(square)]];
			#endif
		}
		static MMINLINE U64 RmagicNOMASK(const unsigned int square, const U64 occupancy)
		{
			#ifndef PERFECT_MAGIC_HASH
				#ifdef MINIMIZE_MAGIC
					return *(magicmoves_r_indices[square]+(((occupancy)*magicmoves_r_magics[square])>>magicmoves_r_shift[square]));
				#else
					return magicmovesrdb[square][((occupancy)*magicmoves_r_magics[square])>>MINIMAL_R_BITS_SHIFT(square)];
				#endif
			#else
				return magicmovesrdb[magicmoves_r_indices[square][((occupancy)*magicmoves_r_magics[square])>>MINIMAL_R_BITS_SHIFT(square)]];
			#endif
		}

		static MMINLINE U64 Qmagic(const unsigned int square,const U64 occupancy)
		{
			return Bmagic(square,occupancy)|Rmagic(square,occupancy);
		}
		static MMINLINE U64 QmagicNOMASK(const unsigned int square, const U64 occupancy)
		{
			return BmagicNOMASK(square,occupancy)|RmagicNOMASK(square,occupancy);
		}
	#else //!USE_INLINING

	#define Qmagic(square, occupancy) (Bmagic(square,occupancy)|Rmagic(square,occupancy))
	#define QmagicNOMASK(square, occupancy) (BmagicNOMASK(square,occupancy)|RmagicNOMASK(square,occupancy))

	#endif //USE_INLINING

	void initmagicmoves(void);
}

#endif //_magicmoveshvesh