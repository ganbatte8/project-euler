#if !defined(COMPILER_MSVC)
#define COMPILER_MSVC 0
#endif
#if !defined(COMPILER_GCC)
#define COMPILER_GCC 0
#endif
#if !defined(COMPILER_LLVM)
#define COMPILER_LLVM 0
#endif

#if !COMPILER_MSVC && !COMPILER_GCC && !COMPILER_LLVM
#if _MSC_VER
#undef COMPILER_MSVC
#define COMPILER_MSVC 1
#endif
#endif

#include <stdint.h>

typedef  int8_t  s8;
typedef  int16_t s16;
typedef  int32_t s32;
typedef  int64_t s64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int b32;
typedef float f32;
typedef double f64;

#define internal static

#define Kilobytes(Value) ((Value) * 1000LL)
#define Megabytes(Value) (Kilobytes(Value) * 1000LL)
#define Gigabytes(Value) (Megabytes(Value) * 1000LL)
#define Terabytes(Value) (Gigabytes(Value) * 1000LL)

#if DEBUG
#define Assert(Expression) if (!(Expression)) {*(int *)0 = 0;}
#define InvalidCodePath {*(int *)0 = 0;}
#define InvalidDefaultCase default: {InvalidCodePath;}
#else
#define Assert(Expression)
#define InvalidCodePath
#define InvalidDefaultCase
#endif

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define false 0
#define true 1

internal void
ZeroBytes(char *Buffer, u32 BytesCount)
{
    for (u32 Byte = 0; Byte < BytesCount; Byte++)
    {
        Buffer[Byte] = 0;
    }
}

typedef struct
{
    u32 Size;
    u8 *Base;
    u32 Used;
    s32 TempCount;
} memory_arena;

internal void
InitializeArena(memory_arena *Arena, u32 Size, void *Base)
{
    Arena->Size = Size;
    Arena->Base = (u8 *)Base;
    Arena->Used = 0;
    Arena->TempCount = 0;
}

#define PushStruct(Arena, type) (type *)PushSize(Arena, sizeof(type))
#define PushArray(Arena, Count, type) (type *)PushSize(Arena, (Count) * sizeof(type))
#define PushZeroStruct(Arena, type) (type *)PushZeroSize(Arena, sizeof(type))
#define PushZeroArray(Arena, Count, type) (type *)PushZeroSize(Arena, (Count) * sizeof(type))


internal void *
PushSize(memory_arena *Arena, u32 Size)
{
    Assert((Arena->Used + Size) <= Arena->Size);
    void *Result = Arena->Base + Arena->Used;
    Arena->Used += Size;
    return Result;
}

internal void *
PushZeroSize(memory_arena *Arena, u32 Size)
{
    Assert((Arena->Used + Size) <= Arena->Size);
    void *Result = Arena->Base + Arena->Used;
    u8 *P = Result;
    Arena->Used += Size;
    while (P < Arena->Base + Arena->Used)
        *P++ = 0;
    return Result;
}

typedef struct 
{
    memory_arena *Arena;
    u32 Used;
} temporary_memory;

internal temporary_memory
BeginTemporaryMemory(memory_arena *Arena)
{
    temporary_memory Result;
    Result.Arena = Arena;
    Result.Used = Arena->Used;
    ++Arena->TempCount;
    return Result;
}

internal void
EndTemporaryMemory(temporary_memory TempMemory)
{
    memory_arena *Arena = TempMemory.Arena;
    Assert(Arena->Used >= TempMemory.Used);
    Arena->Used = TempMemory.Used;
    --Arena->TempCount;
    Assert(Arena->TempCount >= 0);
}

internal void
CheckArena(memory_arena *Arena)
{
    Assert(Arena->TempCount == 0);
}

internal void
SubArena(memory_arena *Result, memory_arena *Arena, u32 Size)
{
    Result->Size = Size;
    Result->Base = (u8 *)PushSize(Arena, Size);
    Result->Used = 0;
    Result->TempCount = 0;
}



internal b32
NumberOfDigitsU32(u32 Number)
{
    // NOTE(vincent): Unused atm
    b32 Result = false;
    u32 NumberOfDigits = 1;
    Assert(Number < 1000 * 1000 * 1000);
    u32 Tens = 10;
    while (Tens < Number)
    {
        Tens *= 10;
        NumberOfDigits++;
    }
    return Result;
}

internal b32 
IsPalindromeU32(u32 Number)
{
    b32 Result = false;
    if (Number < 1000000)
    {
        Number = (Number % 100001) % 10010;
        Result = (Number % 1100 == 0);
    }
    else if (Number > 10000)
    {
        Number = (Number % 10001) % 1010;
        Result = (Number % 100) == 0;
    }
    else
        InvalidCodePath;
    return Result;
}

#include <stdio.h>


internal void
Problem001()
{
    // problem 1: sum of all positive multiples of 3 or 5 below 1000.
    // This could be done on pen and paper...
    u32 Sum = 0;
    for (int k = 1; k < 1000; k++)
    {
        if (k % 3 == 0 || k % 5 == 0)
            Sum += k;
    }
    printf("%d\n", Sum);
}

internal void
Problem002()
{
    // problem 2: Sum of even-valued Fibonacci numbers below 4 million.
    u32 Fib0 = 0;
    u32 Fib1 = 1;
    u32 Result002 = 0;
    while (Fib1 < 4000 * 1000)
    {
        if (Fib1 & 1)
            Result002 += Fib1;
        u32 Temp = Fib0 + Fib1;
        Fib0 = Fib1;
        Fib1 = Temp;
    }
    printf("%d\n", Result002);
}

internal void
Problem003()
{
    // problem 3: largest prime factor of 600851475143 
    u64 BigNumber = 600851475143;
    u64 Divisor = 3;
    for (; BigNumber > 1; Divisor+=2)
    {
        while (BigNumber % Divisor == 0)
            BigNumber /= Divisor;
    }
    Divisor -= 2;
    printf("%lld\n", Divisor);
}

internal void
Problem004()
{
    // problem 4: Largest palindrome made from the product of two 3-digit numbers.
    u32 BigFactor = 999;
    u32 SmallFactor = 999;
    u32 BiggestPalindrome = 0;
    
    for (;;)
    {
        u32 Product = BigFactor * SmallFactor;
        if (Product < BiggestPalindrome || SmallFactor <= 100)
        {
            if (BigFactor == 100 || BigFactor == SmallFactor)
                break;
            BigFactor--;
            SmallFactor = BigFactor;
        }
        else if (IsPalindromeU32(Product))
        {
            BiggestPalindrome = Product;
            BigFactor--;
            SmallFactor = BigFactor;
        }
        else
        {
            SmallFactor--;
        }
    }
    printf("%d\n", BiggestPalindrome);
}

internal void
Problem005()
{
    // problem 5: lcm(1, 2, ..., 20)
    u64 WorkingFactor = 2;
    u64 LastLCM = 1;
    u64 Result005 = 1;
    for (;;)
    {
        Result005 += LastLCM;
        while (Result005 % WorkingFactor == 0)
        {
            WorkingFactor++;
            if (WorkingFactor > 20)
                goto LABEL_DONE_005;
            LastLCM = Result005;
        }
    }
    LABEL_DONE_005:
    printf("%lld\n", Result005);
}

internal void
Problem006()
{
    // problem 6: difference between sum of squares and square of sum of 1,2,...,100
    u32 SquareOfSum = 50*101*50*101;
    u32 SumOfSquares = (100*101*201)/6;
    u32 Difference = SquareOfSum - SumOfSquares;
    printf("%d\n", Difference);
}

internal b32
IsPrime(u32 N)
{
    Assert(N > 5);
    b32 Result = true;
    u32 Mod6 = N % 6;
    if (Mod6 == 1 || Mod6 == 5)
    {
        for (u32 Divisor = 5; Divisor*Divisor <= N; Divisor+=2)
        {
            if (N % Divisor == 0)
            {
                Result = false;
                break;
            }
        }
    }
    else
        Result = false;
    
    return Result;
}

internal void
Problem007()
{
    // problem 7: 10001st prime number
    u32 NextCandidate = 5;
    u32 PrimesCount = 3;
    while (PrimesCount < 10001)
    {
        NextCandidate += 2;
        while(!IsPrime(NextCandidate))
        {
            NextCandidate += 2;
        }
        PrimesCount++;
    }
    printf("%d\n", NextCandidate);
}

internal void
Problem008()
{
    // Find the largest product given by 13 adjacent digits in the following 1000-digit number.
    char *Number = 
        "73167176531330624919225119674426574742355349194934"
        "96983520312774506326239578318016984801869478851843"
        "85861560789112949495459501737958331952853208805511"
        "12540698747158523863050715693290963295227443043557"
        "66896648950445244523161731856403098711121722383113"
        "62229893423380308135336276614282806444486645238749"
        "30358907296290491560440772390713810515859307960866"
        "70172427121883998797908792274921901699720888093776"
        "65727333001053367881220235421809751254540594752243"
        "52584907711670556013604839586446706324415722155397"
        "53697817977846174064955149290862569321978468622482"
        "83972241375657056057490261407972968652414535100474"
        "82166370484403199890008895243450658541227588666881"
        "16427171479924442928230863465674813919123162824586"
        "17866458359124566529476545682848912883142607690042"
        "24219022671055626321111109370544217506941658960408"
        "07198403850962455444362981230987879927244284909188"
        "84580156166097919133875499200524063689912560717606"
        "05886116467109405077541002256983155200055935729725"
        "71636269561882670428252483600823257530420752963450";
    
    u32 FirstDigitOffset = 0;
    u32 LastDigitOffset = 0;
    u64 LargestProduct = 1;
    u64 CurrentProduct = 1;
    while (LastDigitOffset < 1000)
    {
        switch (Number[LastDigitOffset])
        {
            case '0':
            FirstDigitOffset = LastDigitOffset + 1;
            CurrentProduct = 1;
            break;
            default:
            {
                u32 ProductDigitCount = LastDigitOffset + 1 - FirstDigitOffset;
                if (ProductDigitCount > 13)
                {
                    CurrentProduct /= (Number[FirstDigitOffset] - '0');
                    FirstDigitOffset++;
                }
                CurrentProduct *= Number[LastDigitOffset] - '0';
                
                if (CurrentProduct > LargestProduct)
                    LargestProduct = CurrentProduct;
            }
        }
        LastDigitOffset++;
    }
    printf("%lld\n", LargestProduct);
}


internal void
Problem009()
{
    // product abc where (a,b,c) is the Pythagorean triplet such that a + b + c = 1000.
    u32 Product = 1;
    for (u32 a = 1; a < 334; a++)
    {
        // NOTE(vincent): right isocele rectangle cannot have integral sides
        for (u32 b = a + 1; 2*b < 1000 - a; b++) 
        {
            u32 c = 1000 - b - a;
            if (c*c == b*b + a*a)
            {
                Product = a*b*c;
                goto Label_Done009;
            }
        }
    }
    Label_Done009:
    printf("%d\n", Product);
}

internal void
Problem010()
{
    // sum of all the primes below two million.
    u64 Sum = 2 + 3 + 5;
    u32 NextCandidate = 7;
    while (NextCandidate < 2000 * 1000)
    {
        if (IsPrime(NextCandidate))
        {
            Sum += NextCandidate;
        }
        NextCandidate += 2;
    }
    printf("%lld\n", Sum);
}

typedef struct 
{
    char *Memory;
    size_t Size;
    b32 Success;
} push_read_file;

internal push_read_file
PushReadFile(memory_arena *Arena, char *Filename)
{
    push_read_file Result = {0};
    
    FILE *File = fopen(Filename, "rb");
    if (File)
    {
        fseek(File, 0, SEEK_END);
        Result.Size = ftell(File);
        fseek(File, 0, SEEK_SET);
        u32 AvailableSize = Arena->Size - Arena->Used;
        if (Result.Size <= AvailableSize)
        {
            Result.Memory = PushArray(Arena, (u32)Result.Size, char);
            size_t BytesWritten = fread(Result.Memory, 1, Result.Size, File);
            if (BytesWritten == Result.Size)
                Result.Success = true;
        }
        fclose(File);
    }
    return Result;
}

internal void
Problem011(memory_arena *Arena)
{
    // Read a 20*20 grid of numbers and find the largest product of four adjacent digits 
    // in one direction (horizontal or vertical or diagonal).
    temporary_memory TempMemory = BeginTemporaryMemory(Arena);
    push_read_file ReadFile = PushReadFile(Arena, "text011");
    Assert(ReadFile.Success && ReadFile.Memory && ReadFile.Size > 0);
    u32 *Grid = PushArray(Arena, 20*20, u32);
    u32 WriteIndex = 0;
    b32 InNumber = true;
    char *At = (char *)ReadFile.Memory;
    char *End = At + ReadFile.Size;
    for (; At < End; ++At)
    {
        if ('0' <= *At && *At <= '9')
        {
            InNumber = true;
            Grid[WriteIndex] = Grid[WriteIndex] * 10 + (*At - '0');
        }
        else
        {
            if (InNumber)
            {
                InNumber = false;
                WriteIndex++;
            }
        }
    }
    Assert(WriteIndex == 400 || WriteIndex == 399);
    u32 LargestProduct = 1;
    
    for (u32 X = 0; X <= 396; X++)
    {
        if (X % 20 < 17)
        {
            u32 HorizontalProduct = Grid[X] * Grid[X+1] * Grid[X+2] * Grid[X+3];
            LargestProduct = HorizontalProduct > LargestProduct ? HorizontalProduct : LargestProduct;
        }
        if (X < 340)
        {
            u32 VerticalProduct = Grid[X] * Grid[X+20] * Grid[X+40] * Grid[X+60];
            LargestProduct = VerticalProduct > LargestProduct ? VerticalProduct : LargestProduct;
        }
        if (X % 20 < 17 && X < 340)
        {
            u32 DescendingDiagonalProduct = Grid[X] * Grid[X+21] * Grid[X+42] * Grid[X+63];
            LargestProduct = DescendingDiagonalProduct > LargestProduct ? 
                DescendingDiagonalProduct : LargestProduct;
        }
        if (X % 20 > 2 && X < 340)
        {
            u32 AscendingDiagonalProduct = Grid[X] * Grid[X+19] * Grid[X+38] * Grid[X+57];
            LargestProduct = AscendingDiagonalProduct > LargestProduct ? 
                AscendingDiagonalProduct : LargestProduct;
        }
    }
    
    printf("%d\n", LargestProduct);
    EndTemporaryMemory(TempMemory);
}


internal void
Problem012()
{
    // smallest triangle number that has over 500 divisors (between 1 and itself, inclusive).
    u32 Triangle = 1;
    u32 NextIncrement = 2;
    
    for (;;)
    {
        Triangle += NextIncrement;
        NextIncrement++;
        
        u32 DivisorCount = 1;
        u32 DivisorCandidate = 2;
        while (DivisorCandidate * DivisorCandidate < Triangle)
        {
            if (Triangle % DivisorCandidate == 0)
            {
                DivisorCount += 2;
            }
            DivisorCandidate++;
        }
        if (DivisorCandidate * DivisorCandidate == Triangle)
            DivisorCount++;
        
        if (DivisorCount > 500)
            break;
    }
    printf("%d\n", Triangle);
}


internal void
Problem013(memory_arena *Arena)
{
    // Text file has a hundred 50-digit numbers, one per line. Work out the first ten digits of the sum.
    temporary_memory TempMemory = BeginTemporaryMemory(Arena);
    push_read_file ReadFile = PushReadFile(Arena, "text013");
    Assert(ReadFile.Success && ReadFile.Memory && ReadFile.Size > 0);
    char *At = (char *)ReadFile.Memory;
    char *End = At + ReadFile.Size;
    
    u64 *DigitSums = PushArray(Arena, 50, u64); 
    s32 WriteIndex = 49;
    for (; At < End; ++At)
    {
        if ('0' <= *At && *At <= '9')
        {
            DigitSums[WriteIndex] += *At - '0';
            WriteIndex = WriteIndex == 0 ? 49 : WriteIndex-1;
        }
    }
    for (u32 Index = 0; Index < 49; Index++)
    {
        u64 Carry = DigitSums[Index] / 10;
        DigitSums[Index + 1] += Carry;
    }
    u64 Result = DigitSums[49];
    u32 JoinIndex = 48;
    while (Result < 1000LLU * 1000 * 1000)
    {
        Result = Result * 10 + (DigitSums[JoinIndex] % 10);
        JoinIndex--;
    }
    while (Result >= 1000LLU * 1000 * 1000 * 10)
    {
        Result /= 10;
    }
    EndTemporaryMemory(TempMemory);
    printf("%lld\n", Result);
}

internal void
Problem014()
{
    // find the starting number < 1 million that produces the longest Collatz chain.
    
    
    u32 BestStartingNumber = 0;
    u32 BestLength = 0;
    u32 CurrentStartingNumber = 1;
    
    while (CurrentStartingNumber < 1000 * 1000)
    {
        u32 CurrentLength = 0;
        
        for (u64 N = CurrentStartingNumber; 
             N != 1; 
             N = (N & 1) ? 3*N + 1 : N >> 1)
        {
            CurrentLength++;
        }
        
        if (CurrentLength > BestLength)
        {
            BestLength = CurrentLength;
            BestStartingNumber = CurrentStartingNumber;
        }
        CurrentStartingNumber++;
        // TODO(vincent): could we skip some starting candidates ?
    }
    
    printf("%d\n", BestStartingNumber);
}

internal void
Problem015()
{
    // 40 choose 20. aka 40! / (20! 20!)
    // https://en.wikipedia.org/wiki/Central_binomial_coefficient
    u64 CentralBinomialCoef = 2;
    for (u32 N = 1; N < 20; N++)
    {
        CentralBinomialCoef *= 4*N + 2;
        CentralBinomialCoef /= N+1;
    }
    printf("%lld\n", CentralBinomialCoef);
    
}

internal void
Problem016()
{
    // sum of digits of 2^1000.
    
    u16 Digits[500] = {0};
    Digits[0] = 1;
    for (u32 Power = 0; Power < 1000; Power++)
    {
        for (u32 Index = 0; Index < ArrayCount(Digits); Index++)
        {
            Digits[Index] *= 2;
        }
        for (u32 Index = 0; Index < ArrayCount(Digits)-1; Index++)
        {
            u16 Carry = Digits[Index] / 10;
            Digits[Index+1] += Carry;
            Digits[Index] %= 10;
        }
    }
    
    u32 DigitSum = 0;
    for (u32 Index = 0; Index < ArrayCount(Digits); Index++)
        DigitSum += Digits[Index];
    
    printf("%d\n", DigitSum);
}


internal void
Problem017()
{
    // count the letters used in: one two three four five... one thousand.
    // Don't count spaces or hyphens.
    
    u8 C[20] = {0};
    C[0] = 0;
    C[1] = 3;   // one
    C[2] = 3;   // two
    C[3] = 5;   // three
    C[4] = 4;   // four
    C[5] = 4;   // five
    C[6] = 3;   // six
    C[7] = 5;   // seven
    C[8] = 5;   // eight
    C[9] = 4;   // nine
    C[10] = 3;  // ten
    C[11] = 6;  // eleven
    C[12] = 6;  // twelve
    C[13] = 8;  // thirteen
    C[14] = 8;  // fourteen
    C[15] = 7;  // fifteen
    C[16] = 7;  // sixteen
    C[17] = 9;  // seventeen
    C[18] = 8;  // eighteen
    C[19] = 8;  // nineteen
    
    u8 TenCount[10];
    TenCount[0] = 0;
    TenCount[1] = 0;
    TenCount[2] = 6; // twenty
    TenCount[3] = 6; // thirty
    TenCount[4] = 5; // forty  (curse you, English!)
    TenCount[5] = 5; // fifty
    TenCount[6] = 5; // sixty
    TenCount[7] = 7; // seventy
    TenCount[8] = 6; // eighty
    TenCount[9] = 6; // ninety
    
    u32 LetterCount = 0;
    LetterCount += 3 + 8; // "one thousand"
    
    for (u32 Number = 1; Number < 1000; Number++)
    {
        u32 HundredUnit = Number / 100;
        u32 HundredRemainder = Number % 100;
        u32 TenUnit = (Number / 10) % 10;
        u32 TenRemainder = Number % 10;
        
        if (HundredUnit > 0)
        {
            LetterCount += C[HundredUnit] + 7;
            if (HundredRemainder > 0)
                LetterCount += 3;  // "and"
        }
        
        if (TenUnit >= 2)
            LetterCount += TenCount[TenUnit] + C[TenRemainder];
        else
            LetterCount += C[HundredRemainder];
    }
    
    printf("%d\n", LetterCount);
}


internal void
Problem018()
{
    // max sum path. https://projecteuler.net/problem=18
    u32 Pyramid[] = {
        75,
        95, 64,
        17, 47, 82,
        18, 35, 87, 10,
        20,  4, 82, 47, 65,
        19,  1, 23, 75,  3, 34,
        88,  2, 77, 73,  7, 63, 67,
        99, 65,  4, 28,  6, 16, 70, 92,
        41, 41, 26, 56, 83, 40, 80, 70, 33,
        41, 48, 72, 33, 47, 32, 37, 16, 94, 29,
        53, 71, 44, 65, 25, 43, 91, 52, 97, 51, 14,
        70, 11, 33, 28, 77, 73, 17, 78, 39, 68, 17, 57,
        91, 71, 52, 38, 17, 14, 91, 43, 58, 50, 27, 29, 48,
        63, 66,  4, 68, 89, 53, 67, 30, 73, 16, 69, 87, 40, 31,
        4 , 62, 98, 27, 23,  9, 70, 98, 73, 93, 38, 53, 60,  4, 23
    };
    
    u32 TriangleNumbers[15];
    for (u32 Index = 0; Index < ArrayCount(TriangleNumbers); Index++)
        TriangleNumbers[Index] = (Index*(Index+1))/2;
    
    /*
0
1 2
3 4 5
6 7 8 9
*/
    
    for (s32 RowIndex = 13; RowIndex >= 0; RowIndex--)
    {
        for (s32 ColIndex = 0; ColIndex <= RowIndex; ColIndex++)
        {
            u32 Index = TriangleNumbers[RowIndex] + ColIndex;
            u32 IndexLeft = TriangleNumbers[RowIndex+1] + ColIndex;
            u32 IndexRight = TriangleNumbers[RowIndex+1] + ColIndex + 1;
            u32 Left = Pyramid[IndexLeft];
            u32 Right = Pyramid[IndexRight];
            u32 Max = Left > Right ? Left : Right;
            Pyramid[Index] += Max;
        }
    }
    
    printf("%d\n", Pyramid[0]);
}

typedef struct
{
    u32 Weekday;  // 1 to 7
    u32 Month;    // 1 to 12
    u32 MonthDay; // 1 to 31
    u32 Year;
} date;

internal void
NextDay(date *Date)
{
    Date->Weekday = Date->Weekday == 7 ? 1 : Date->Weekday + 1;
    Date->MonthDay++;
    switch (Date->Month)
    {
        case 4:
        case 6:
        case 9:
        case 11:
        {
            if (Date->MonthDay == 31)
            {
                Date->MonthDay = 1;
                Date->Month++;
            }
        } break;
        case 2:
        {
            b32 LeapYear = ( ((Date->Year % 4) == 0 && (Date->Year % 100 != 0)) ||
                            Date->Year % 400 == 0 );
            if (Date->MonthDay == 30 || (Date->MonthDay == 29 && !LeapYear))
            {
                Date->MonthDay = 1;
                Date->Month = 3;
            }
        } break;
        default:
        {
            if (Date->MonthDay == 32)
            {
                Date->Month = Date->Month == 12 ? 1 : Date->Month+1; 
                if (Date->Month == 1)
                    Date->Year++;
                Date->MonthDay = 1;
            }
        }
    }
}

internal void
Problem019()
{
    // How many Sundays fell on the first of the month during the twentieth century 
    // (1 Jan 1901 to 31 Dec 2000)?
    /*
1 Jan 1900 was a Monday.
Thirty days has September,
April, June and November.
All the rest have thirty-one,
Saving February alone,
Which has twenty-eight, rain or shine.
And on leap years, twenty-nine.
A leap year occurs on any year evenly divisible by 4, but not on a century 
unless it is divisible by 400.
*/
    
    date Date;
    Date.Weekday = 1;
    Date.Month = 1;
    Date.MonthDay = 1;
    Date.Year = 1900;
    u32 SundayOnFirstCount = 0;
    while (Date.Year <= 2000)
    {
        if (Date.Year >= 1901 && Date.MonthDay == 1 && Date.Weekday == 7)
            SundayOnFirstCount++;
        NextDay(&Date);
    }
    printf("%d\n", SundayOnFirstCount);
}

internal void
Problem020()
{
    // Sum of digits of 100!
    // 100! has less than 200 digits.
    // a u32 should be able to hold 6 digits while being able to avoid overflow when multiplying by 100.
    // We could store the "result" of 100! in thirty-three u32s or less.
    
    u32 Product[33] = {0};
    Product[0] = 1;
    
    for (u32 Factor = 1; Factor < 100; Factor++)
    {
        for (u32 Index = 0; Index < ArrayCount(Product); Index++)
            Product[Index] *= Factor;
        
        u32 Carry = 0;
        for (u32 Index = 0; Index < ArrayCount(Product); Index++)
        {
            Carry = Product[Index] / (1000 * 1000);
            if (Carry > 0)
            {
                Assert(Index < ArrayCount(Product) - 1);
                Product[Index] %= 1000 * 1000;
                Product[Index+1] += Carry;
            }
        }
    }
    
    u32 SumDigits = 0;
    for (u32 Index = 0; Index < ArrayCount(Product); Index++)
    {
        for (u32 N = Product[Index]; N > 0; N /= 10)
            SumDigits += N % 10;
    }
    
    printf("%d\n", SumDigits);
}

internal u32
SumProperDivisors(u32 N)
{
    u32 Result = 0;
    if (N > 1)
    {
        Result = 1;
        for (u32 DivisorCandidate = 2; DivisorCandidate*DivisorCandidate <= N; DivisorCandidate++)
        {
            if (N % DivisorCandidate == 0)
            {
                Result += DivisorCandidate;
                if (DivisorCandidate * DivisorCandidate != N)
                {
                    u32 BroDivisor = N / DivisorCandidate;
                    Result += BroDivisor;
                }
            }
        }
    }
    return Result;
}

internal void
Problem021()
{
    // Sum of all amicable numbers < 10000.
    // d(n): sum of proper divisors (divisors between 1 inclusive and n exclusive).
    // Distinct naturals a and b form an amicable pair when d(a) = b and d(b) = a.
    
    u32 SumAmicables = 0;
    
    for (u32 N = 2; N < 10000; N++)
    {
        u32 SumPD = SumProperDivisors(N);
        if (SumPD != N)
        {
            u32 OtherSum = SumProperDivisors(SumPD);
            if (OtherSum == N)
                SumAmicables += N;
        }
    }
    
    printf("%d\n", SumAmicables);
}

typedef struct
{
    char *Base;
    u32 Size;
} string;

internal b32
StringLEQ(string A, string B)
{
    b32 Result = A.Size <= B.Size;
    u32 MinSize = A.Size < B.Size ? A.Size : B.Size;
    for (u32 Index = 0; Index < MinSize; Index++)
    {
        if (A.Base[Index] > B.Base[Index])
        {
            Result = false;
            break;
        }
        if (A.Base[Index] < B.Base[Index])
        {
            Result = true;
            break;
        }
    }
    return Result;
}

internal string
StringFromLiteral(char *Literal)
{
    string Result;
    Result.Base = Literal;
    Result.Size = 0;
    while (*Literal)
    {
        Literal++;
        Result.Size++;
    }
    return Result;
}

internal void
Problem022(memory_arena *Arena)
{
    // Read the names in the file, sort them, work out the "alphabetical value" of each name
    // e.g. COLIN is worth 3 + 15 + 12 + 9 + 14 = 53.
    // Then, multiply that value by its position in the sorted list to get the name score.
    // Sum all the name scores.
    //string Kathleen = StringFromLiteral("KATHLEEN");
    //string Pamela = StringFromLiteral("PAMELA");
    //string Earnest = StringFromLiteral("EARNEST");
    //string Guillermo = StringFromLiteral("GUILLERMO");
    //Assert(StringLEQ(Earnest, Guillermo));
    //Assert(!StringLEQ(Guillermo, Earnest));
    u32 SumNameScores = 0;
    
    temporary_memory TempMemory = BeginTemporaryMemory(Arena);
    push_read_file ReadFile = PushReadFile(Arena, "text022");
    Assert(ReadFile.Success && ReadFile.Memory && ReadFile.Size > 0);
    char *At = (char *)ReadFile.Memory;
    char *End = At + ReadFile.Size;
    
    b32 InName = false;
    string *Name = 0;
    u32 StringsCount = 0;
    string *NameStrings = (string *)((u8 *)Arena->Base + Arena->Used);
    
    for (; At < End; ++At)
    {
        switch(*At)
        {
            case '"':
            InName = !InName;
            if (InName)
            {
                Name = PushStruct(Arena, string);
                Name->Base = At + 1;
                StringsCount++;
            }
            else
                Name->Size = (u32)(At - Name->Base);
            break;
        }
    }
    
    // TODO(vincent): Can probably do better than bubble sort
    for (u32 A = StringsCount-1; A > 0; A--)
    {
        for (u32 B = 0; B < A; B++)
        {
            if (!StringLEQ(NameStrings[B], NameStrings[B+1]))
            {
                string Temp = NameStrings[B];
                NameStrings[B] = NameStrings[B+1];
                NameStrings[B+1] = Temp;
            }
        }
    }
    
    for (u32 A = 0; A < StringsCount-1; A++)
    {
        Assert(StringLEQ(NameStrings[A], NameStrings[A+1]));
        Assert(!StringLEQ(NameStrings[A+1], NameStrings[A]));
    }
    
    for (u32 Index = 0; Index < StringsCount; Index++)
    {
        u32 NameScore = 0;
        string Name = NameStrings[Index];
        for (u32 Index = 0; Index < Name.Size; Index++)
        {
            NameScore += Name.Base[Index] - 'A' + 1;
        }
        NameScore *= Index+1;
        SumNameScores += NameScore;
    }
    
    EndTemporaryMemory(TempMemory);
    printf("%d\n", SumNameScores);
}


internal void
Problem023(memory_arena *Arena)
{
    // Perfect number: equal to the sum of its proper divisors. (e.g. 28)
    // Deficient when sum is inferior, abundant when sum is superior.
    // Property: All integers > 28123 can be written as the sum of two abundant numbers.
    // Find the sum of all positive integers which cannot be written as the sum of two abundant numbers.
    temporary_memory TempMem = BeginTemporaryMemory(Arena);
    
    Assert(Arena->Used == 0);
    u32 *AbundantNumbers = (u32 *)(Arena->Base + Arena->Used);
    u32 AbundantCount = 0;
    for (u32 AbundantCandidate = 2; AbundantCandidate < 28123; AbundantCandidate++)
    {
        if (SumProperDivisors(AbundantCandidate) > AbundantCandidate)
        {
            u32 *Abundant = PushStruct(Arena, u32);
            *Abundant = AbundantCandidate;
            AbundantCount++;
        }
    }
    
    // TODO(vincent): I wonder how much of a speed difference it makes 
    // between b32, u8 and bit lookup table. I suspect compressed tables could be faster because
    // less cache misses / less memory bandwidth pressure?
    b32 *IsAbundantTable = PushArray(Arena, 28123, b32);
    for (u32 Number = 0; Number < 28123; Number++)
    {
        IsAbundantTable[Number] = false;
    }
    for (u32 AbundantIndex = 0; AbundantIndex < AbundantCount; AbundantIndex++)
    {
        
        IsAbundantTable[AbundantNumbers[AbundantIndex]] = true;
    }
    
    u32 Result = 1;
    for (u32 SumCandidate = 2; SumCandidate < 28123; SumCandidate++)
    {
        b32 IsSumOfTwo = false;
        for (u32 AbundantIndex = 0; AbundantIndex < AbundantCount; AbundantIndex++)
        {
            u32 AbundantTerm = AbundantNumbers[AbundantIndex];
            if (AbundantTerm >= SumCandidate)
                break;
            u32 Difference = SumCandidate - AbundantTerm;
            if (IsAbundantTable[Difference])
            {
                IsSumOfTwo = true;
                break;
            }
        }
        if (!IsSumOfTwo)
        {
            Result+= SumCandidate;
        }
    }
    printf("%d\n", Result);
    EndTemporaryMemory(TempMem);
}


internal void
Problem024()
{
    // Millionth lexicographic permutation of the digits 0,...,9.
    
    // NOTE(vincent): What's the index of the first lexicographic permutation that begins with a 1 ?
    // There are 9! permutations before that, which is 326880. So it's the 326881st perm.
    // What's the first digit of the millionth permutation ?
    // Since 1000000 divided by 9! gives the quotient 3, 
    // and the fourth smallest digit available is 3, it's 3.
    // Work with the remainder and reiterate. 
    
    u64 Result = 0;
    b32 DigitIsUsed[10] = {0};
    u32 Factorials[10];
    Factorials[0] = 1;
    for (u32 Index = 1; Index < ArrayCount(Factorials); Index++)
    {
        Factorials[Index] = Factorials[Index-1]*Index;
    }
    
    u32 IndexToConsume = 999999;
    for (s32 FactorialIndex = 9; FactorialIndex >= 0; FactorialIndex--)
    {
        s32 AvailableOffsetTarget = IndexToConsume / Factorials[FactorialIndex];
        IndexToConsume -= AvailableOffsetTarget * Factorials[FactorialIndex];
        s32 Digit = -1;
        for (s32 AvailableOffset = -1; AvailableOffset < AvailableOffsetTarget; )
        {
            Digit++;
            Assert(0 <= Digit && Digit < 10);
            if (!DigitIsUsed[Digit])
            {
                AvailableOffset++;
            }
        }
        
        DigitIsUsed[Digit] = true;
        Result = Result*10 + Digit;
    }
    Assert(IndexToConsume == 0);
    
    printf("%lld\n", Result);
}


internal void
Problem025()
{
    // Index of the first Fibonacci term to contain 1000 digits.
    // Initial terms are F1 = 1, F2 = 1.
    
    u32 FibIndex = 1;
    // A 32-bit unsigned integer can go up to 4 billion, or 9-10 digits. We will use that to represent
    // 9-digit buckets of big Fibonacci numbers.
    // NOTE: 1000 = 9*111 + 1.
    u32 FibValue[112] = {0};  // lower index maps to least significant chunk
    FibValue[0] = 1;
    u32 PreviousFibValue[112] = {0};
    
    while (FibValue[111] == 0)
    {
        b32 Carry = false;
        for (u32 BucketIndex = 0; BucketIndex < ArrayCount(FibValue); BucketIndex++)
        {
            u32 Temp = FibValue[BucketIndex];
            FibValue[BucketIndex] += PreviousFibValue[BucketIndex] + Carry;
            if (FibValue[BucketIndex] >= 1000 * 1000 * 1000)
            {
                Assert(BucketIndex+1 < ArrayCount(FibValue));
                Assert(FibValue[BucketIndex] < 2000 * 1000 * 1000);
                FibValue[BucketIndex] -= 1000 * 1000 * 1000;
                Carry = true;
            }
            else
                Carry = false;
            PreviousFibValue[BucketIndex] = Temp;
        }
        FibIndex++;
    }
    
    printf("%u\n", FibIndex);
}


internal void
Problem026(memory_arena *Arena)
{
    // Find the value of d < 1000 for which 1/d contains the longest recurring cycle in its decimal part.
    
    u32 LongestCycleLength = 0;
    u32 Result = 0;
    
    // NOTE(vincent): Table lookup to avoid n^2 pair search most of the time, runs much much faster
    b32 RemaindersLookup[1000] = {false};  
    
    for (u32 Divisor = 2; Divisor < 1000; Divisor++)
    {
        temporary_memory RemaindersMemory = BeginTemporaryMemory(Arena);
        
        u32 *Remainders = (u32 *)(Arena->Base + Arena->Used);
        u32 RemaindersCount = 0;
        u32 Dividend = 1;
        u32 CycleLength = 0;
        for (u32 RemaindersLookupIndex = 0; RemaindersLookupIndex < ArrayCount(RemaindersLookup);
             RemaindersLookupIndex++)
            RemaindersLookup[RemaindersLookupIndex] = false;
        while (Dividend > 0 && CycleLength == 0)
        {
            u32 Quotient = Dividend / Divisor;
            Assert(Quotient < 10);
            Dividend -= Quotient * Divisor;
            
            *PushStruct(Arena, u32) = Dividend;
            ++RemaindersCount;
            Assert(Dividend < ArrayCount(RemaindersLookup));
            if (RemaindersLookup[Dividend])
            {
                for (u32 RemainderIndex = 0; RemainderIndex < RemaindersCount; ++RemainderIndex)
                {
                    for (u32 OtherRemainder = RemainderIndex + 1; OtherRemainder < RemaindersCount; 
                         ++OtherRemainder)
                    {
                        if (Remainders[RemainderIndex] == Remainders[OtherRemainder])
                        {
                            CycleLength = OtherRemainder - RemainderIndex;
                            if (CycleLength > LongestCycleLength)
                            {
                                LongestCycleLength = CycleLength;
                                Result = Divisor;
                            }
                        }
                        
                    }
                }
            }
            else 
                RemaindersLookup[Dividend] = true;
            
            Dividend *= 10;
        }
        EndTemporaryMemory(RemaindersMemory);
    }
    
    printf("%u\n", Result);
}

internal void
Problem027()
{
    // Considering quadratics of the form n^2 + an + b where |a| < 1000 and |b| <= 1000,
    // Find the one that produces the most prime numbers for consecutive values of n, starting at
    // n = 0, and return a*b.
    
    // NOTE(vincent): In order for f(n) = n^2 + an + b to be a prime number many consecutive times,
    // it must be an odd number many consecutive times.
    // When n is even, f(n) has the same parity as b, therefore b must be odd.
    // When n is odd, then n^2 is odd, thus n^2 + b is even, thus f(n) has the same parity as a.
    // Therefore a must be odd as well.
    
    // Note also that we want b = f(0) to be a prime number. b must be positive.
    
    s32 BestN = 0;
    s32 BestProduct = 1;
    for (s32 b = 3; b <= 999; b+=2)
    {
        if (b == 3 || b == 5 || IsPrime(b))
        {
            for (s32 a = -999; a <= 999; a+=2)
            {
                s32 n = 0;
                for (;;)
                {
                    s32 f = n*n + a*n + b;
                    if (f == 3 || f == 5 || (f > 5 && IsPrime(f)))
                        n++;
                    else
                        break;
                }
                if (n > BestN)
                {
                    BestN = n;
                    BestProduct = a*b;
                }
            }
        }
    }
    
    printf("%d\n", BestProduct);
}


internal void
Problem028()
{
    /* 
Consider a spiral of numbers formed like this:
21 22 23 24 25
20  7  8  9 10
19  6  1  2 11
18  5  4  3 12
17 16 15 14 13
Keep building layers until you get a 1001 by 1001 spiral, and sum the diagonal elements (crossing 1).
*/
    
    /*
When you build each consecutive layer, the terms to add look like this:
1           (initial layer)
3 5 7 9     (increment by 2 each time)
13 17 21 25 (increment by 4 each time)
 ...         (increment by 6 each time)

including the initial layer there are 501 such layers.
*/
    
    u32 Sum = 1;
    u32 LastTerm = 1;
    for (u32 LayerIndex = 1; LayerIndex <= 500; LayerIndex++)
    {
        u32 TermIncrement = LayerIndex*2;
        Sum += 4*LastTerm + 10*TermIncrement;  // S + (x+t) + (x+2t) + (x+3t) + (x+4t) = S + 4x + 10t
        LastTerm += 4*TermIncrement;
    }
    printf("%u\n", Sum);
}


internal void
Problem029()
{
    // How many distinct terms are in the sequence generated by a^b
    // for 2 <= a <= 100 and 2 <= b <= 100 ?
    
    // If these were all distinct, there would be 99*99 = 9801 such terms.
    u32 Result = 99*99;
    // We end up with duplicates when a is the strict power of another number.
    // We can use a sieve approach like this:
    // strict powers of 2: 4 8 16 32 64
    // strict powers of 3: 9 27 81
    // (4 already taken, was power of 2)
    // strict powers of 5: 25
    // strict powers of 6: 36
    // strict powers of 7: 49
    // (8 already taken, was power of 2)
    // (9 already taken, was power of 3)
    // strict powers of 10: 100
    // strict powers of 11: (all > 100, none that you would encounter as another value of a)
    // ...
    // strict powers of 99: (all > 100, none that you would encounter as another value of a)
    // (100 already taken, was power of 100)
    
    // The number of strict power integers <= 100 is 12.  
    // Let's subtract the 12*99 candidates associated with those values of a.
    Result -= 12*99;
    // Cases a = 2, 3, 5, 6, 7, 10 can be treated specially where b can take some values beyond 100.
    // The critical part is counting how many such values of b there are for each case.
    // [i] a = 2: 
    // To account for a = 2 and all the strict powers of 2, 
    // we want to count the cardinal of the union of these sets:
    // {2^b, 2 <= b <= 100}, {2^(2b), 2 <= b <= 100}, ..., {2^(6b), 2 <= b <= 100}
    // or
    // {b, 2 <= b <= 100}, {2b, 2 <= b <= 100}, ..., {6b, 2 <= b <= 100}
    
    // You should end up with a cardinal n > 99 (because b has been extended).
    // We've already accounted for 99 of those possible values of b,
    // so we will apply Result += (n-99);
    
    u32 Count2 = 0;  // case a = 2
    u32 Count3 = 0;  // case a = 3
    u32 Count5 = 0;  // cases a = 5, 6, 7 and 10
    b32 Lookup2[601] = {false};
    b32 Lookup3[401] = {false};
    b32 Lookup5[201] = {false};
    for (u32 b = 2; b <= 100; ++b)
    {
        for (u32 x = b; x <= 6*b; x += b)
        {
            if (!Lookup2[x]) 
            {
                ++Count2; 
                Lookup2[x] = true;
            }
        }
        for (u32 x = b; x <= 4*b; x += b)
        {
            if (!Lookup3[x]) 
            {
                ++Count3; 
                Lookup3[x] = true;
            }
        }
        for (u32 x = b; x <= 2*b; x += b)
        {
            if (!Lookup5[x]) 
            {
                ++Count5; 
                Lookup5[x] = true;
            }
        }
    }
    Assert(Count2 > 100 && Count3 > 100 && Count5 > 100);
    Result += (Count2-99) + (Count3-99) + 4*(Count5-99);
    
    printf("%u\n", Result);
}

internal void
Problem030()
{
    // Sum of all integers > 1 that are the sum of the fifth powers of their digits.
    
    // Let d be a digit. Then 1 <= d^5 <= 9^5 = 59049.
    // For some integer value n and beyond, we have 10^(n-1)> n*59041.
    // i.e. The smallest number with n digits is bigger 
    // than the biggest sum of fifth powers an n-digit number could have.
    // This is the case iff n >= 7, so we are dealing with numbers inferior to 1 million.
    
    u32 FifthPowers[10] = {0,1,32,243,1024,3125,7776,16807,32768,59049};
    u32 Result = 0;
    for (u32 Number = 2; Number < 1000 * 1000; Number++)
    {
        u32 Sum = 0;
        u32 Copy = Number;
        while (Copy > 0)
        {
            u32 Digit = Copy % 10;
            Sum += FifthPowers[Digit];
            Copy /= 10;
        }
        if (Sum == Number)
            Result += Sum;
    }
    
    printf("%u\n", Result);
}

internal void
Problem031()
{
    // How many ways can we decompose 200 (coins) as a sum of the terms 1, 2, 5, 10, 20, 50, 100, 200?
    
    // Use dynamic programming.
    // f(n, t) : The answer when you have n coins at stage t
    // t = 0: biggest possible term is 200
    // t = 1: biggest possible term is 100
    // t = 2: biggest possible term is 50
    // t = 3: biggest possible term is 20
    // t = 4: biggest possible term is 10
    // t = 5: biggest possible term is 5
    // t = 6: biggest possible term is 2
    // t = 7: biggest possible term is 1
    // We want to compute f(200, 0).
    // if t = 7 or n = 0, then f(n, t) = 1.
    // else if n >= term(t) then f(n, t) = f(n, t+1) + f(n-term(t), t).
    // else, f(n, t) = f(n, t+1).
    
    s32 Terms[8] = {200, 100, 50, 20, 10, 5, 2, 1};
    u32 Results[202*8] = {0};  // element (n, t) accessed by index 202*t + n.
    
    for (u32 n = 0; n <= 200; n++)
        Results[202*7 + n] = 1; 
    for (u32 t = 0; t <= 7; t++)
        Results[202*t + 0] = 1;
    
    for (s32 n = 1; n <= 200; n++)
    {
        for (s32 t = 6; t >= 0; t--)
        {
            Assert(Results[202*t + n] == 0);
            if (n >= Terms[t])
                Results[202*t + n] = Results[202*(t+1) + n] + Results[202*t + (n-Terms[t])];
            else
                Results[202*t + n] = Results[202*(t+1) + n];
        }
    }
    
    printf("%d\n", Results[202*0 + 200]);
}

internal b32
MarkNewDigits(b32 Number, b32 *FoundDigit)
{
    b32 Result = true;
    while (Number > 0)
    {
        u32 Digit = Number % 10;
        if (FoundDigit[Digit])
        {
            Result = false;
            break;
        }
        else
        {
            FoundDigit[Digit] = true;
            Number /= 10;
        }
    }
    return Result;
}

internal void
CheckAndAddPandigital(u32 Product, b32 *FoundDigit, u32 *Pandigitals, u32 *PandigitalCount, 
                      memory_arena *Arena) 
{
    b32 Pandigital = true;
    for (u32 Digit = 0; Digit < 10; Digit++)
        if (!FoundDigit[Digit]) Pandigital = false;
    if (Pandigital)
    {
        b32 NewPandigital = true;
        for (u32 Index = 0; Index < *PandigitalCount; Index++)
        {
            if (Pandigitals[Index] == Product)
            {
                NewPandigital = false;
                break;
            }
        }
        if (NewPandigital)
        {
            *PushStruct(Arena, u32) = Product;
            *PandigitalCount += 1;
        }
    }
}

internal void
Problem032(memory_arena *Arena)
{
    // Find the sum of all products whose multiplicand/multiplier/product identity can be written
    // as 1 through 9 pandigital (e.g. 7254 is one, because 39*186=7254)
    
    // To explore candidates we are looking for integers a,b,c such that a*b=c with 0 < a < b < c.
    // a is at most 3 digits long.
    // If a is one digit, then b is exactly 4 digits.
    // If a is two digits, then b is 3 or 4 digits.
    // If a is three digits, then b must be 3 digits as well.
    b32 FoundDigit[10] = {false};
    FoundDigit[0] = true;   // think of 0 as always being taken, because expressions shouldn't have any.
    temporary_memory PandigitalMemory = BeginTemporaryMemory(Arena);
    
    u32 *Pandigitals = (u32 *)(Arena->Base + Arena->Used);
    u32 PandigitalCount = 0;
    for (u32 a = 1; a < 10; ++a)
    {
        for (u32 b = 1000; b < 10000; ++b)
        {
            for (u32 Digit = 1; Digit < ArrayCount(FoundDigit); ++Digit)
                FoundDigit[Digit] = false;
            FoundDigit[a] = true;
            u32 Product = a*b;
            b32 NoSameDigits = MarkNewDigits(b, FoundDigit) && MarkNewDigits(Product, FoundDigit);
            if (NoSameDigits)
                CheckAndAddPandigital(Product, FoundDigit, Pandigitals, &PandigitalCount, Arena); 
        }
    }
    
    for (u32 a = 10; a < 100; ++a)
    {
        u32 ADigit0 = a % 10;
        u32 ADigit1 = a / 10;
        if (ADigit0 == ADigit1 || ADigit0 == 0 || ADigit1 == 0)
            continue;
        for (u32 b = 100; b < 1000; ++b)
        {
            for (u32 Digit = 1; Digit < ArrayCount(FoundDigit); ++Digit)
                FoundDigit[Digit] = false;
            FoundDigit[ADigit0] = true;
            FoundDigit[ADigit1] = true;
            u32 Product = a*b;
            b32 NoSameDigits = MarkNewDigits(b, FoundDigit) && MarkNewDigits(Product, FoundDigit);
            if (NoSameDigits)
                CheckAndAddPandigital(Product, FoundDigit, Pandigitals, &PandigitalCount, Arena);
        }
    }
    
    for (u32 a = 100; a < 1000; ++a)
    {
        u32 ADigit0 = a % 10;
        u32 ADigit1 = (a / 10) % 10;
        u32 ADigit2 = a / 100;
        if (ADigit0 == ADigit1 || ADigit1 == ADigit2 || ADigit2 == ADigit0 ||
            ADigit0 == 0 || ADigit1 == 0 || ADigit2 == 0)
            continue;
        for (u32 b = a+1; b < 1000; ++b)
        {
            for (u32 Digit = 1; Digit < ArrayCount(FoundDigit); ++Digit)
                FoundDigit[Digit] = false;
            FoundDigit[ADigit0] = true;
            FoundDigit[ADigit1] = true;
            FoundDigit[ADigit2] = true;
            u32 Product = a*b;
            b32 NoSameDigits = MarkNewDigits(b, FoundDigit) && MarkNewDigits(Product, FoundDigit);
            if (NoSameDigits)
                CheckAndAddPandigital(Product, FoundDigit, Pandigitals, &PandigitalCount, Arena);
        }
    }
    
    u32 Result = 0;
    for (u32 Index = 0; Index < PandigitalCount; ++Index)
        Result += Pandigitals[Index];
    printf("%u\n", Result);
    EndTemporaryMemory(PandigitalMemory);
}


internal u32
GCD(u32 A, u32 B)
{
    Assert(B < A);
    
    for (;;)
    {
        u32 Quotient = A / B;
        u32 Remainder = A % B;
        
        if (Remainder == 0)
            break;
        
        A = B;
        B = Remainder;
    }
    
    return B;
}

internal u64
GCD64(u64 A, u64 B)
{
    Assert(B < A);
    for (;;)
    {
        u64 Quotient = A / B;
        u64 Remainder = A % B;
        if (Remainder == 0)
            break;
        A = B;
        B = Remainder;
    }
    return B;
}


internal void
Problem033()
{
    // Find the four "non-trival" fractions, with two digits in the numerator and the denominator,
    // lower than 1 in value, that can be simplified by cancelling a digit like 49/98 = 4 / 8.
    // 30 / 50 is considered "trivial".
    // Return the value of the denominator of the product of these four fractions 
    // in its lowest common terms.
    
    u32 DenominatorProduct = 1;
    u32 NumeratorProduct = 1;
    for (u32 Numerator = 11; Numerator < 99; ++Numerator)
    {
        u32 NumHigh = Numerator / 10;
        u32 NumLow = Numerator % 10;
        if (NumLow == 0)
            continue;
        for (u32 Denominator = Numerator+1; Denominator < 100; ++Denominator)
        {
            u32 DenHigh = Denominator / 10;
            u32 DenLow = Denominator % 10;
            if (DenLow == 0)
                continue;
            
            if (DenHigh == NumLow)
            {
                if (DenLow * Numerator == Denominator * NumHigh)
                {
                    //printf("%d over %d\n", Numerator, Denominator);
                    NumeratorProduct *= Numerator;
                    DenominatorProduct *= Denominator;
                }
                
            }
            if (DenLow == NumHigh)
            {
                if (DenHigh * Numerator == Denominator * NumLow)
                {
                    //printf("%d over %d\n", Numerator, Denominator);
                    NumeratorProduct *= Numerator;
                    DenominatorProduct *= Denominator;
                }
            }
        }
    }
    
    u32 Result = DenominatorProduct / GCD(DenominatorProduct, NumeratorProduct);
    
    printf("%u\n", Result);
}

internal void
Problem034()
{
    // Find the sum of all numbers which are equal to the sum of the factorial of their digits.
    // 1 and 2 are not included.
    u32 Factorials[10] = {1,1,2,6,24,120,720,5040,40320,362880};
    
    // the maximum factorial value you could get for an n-digit number is 362880 * n,
    // and the minimum value of an n-digit number is 10^(n-1).
    // We have 362880*n < 10^(n-1) iff n >= 8, so we are dealing with numbers < 10^7.
    
    u32 Result = 0;
    for (u32 Number = 10; Number <= 7*362880; ++Number)
    {
        u32 CopyNumber = Number;
        u32 SumOfFactorials = 0;
        while (CopyNumber > 0)
        {
            SumOfFactorials += Factorials[CopyNumber % 10];
            CopyNumber /= 10;
        }
        if (SumOfFactorials == Number)
        {
            Result += Number;
        }
    }
    printf("%u\n", Result);
}


internal u32
BuildRotatedNumber(u32 *State, u32 DigitCount, u32 Offset)
{
    u32 PowerTen = 1;
    u32 Result = 0;
    // if Offset == 0       use indices  0 1 2 3 ... DigitCount-1
    // if Offset == 1       use indices  1 2 3 ... DigitCount-1 0
    for (u32 BuildingDigit = 0; BuildingDigit < DigitCount; ++BuildingDigit)
    {
        Result += PowerTen * State[Offset];
        ++Offset;
        if (Offset == DigitCount)
            Offset = 0;
        PowerTen *= 10;
    }
    return Result;
}

internal b32
StateAdvance035(u32 *State)
{
    b32 Advanced = false;
    
    u32 IndexToIncrement = 0;
    for (;;)
    {
        switch (State[IndexToIncrement])
        {
            case 0: State[IndexToIncrement] = 1; Advanced = true; break;
            case 1: State[IndexToIncrement] = 3; Advanced = true; break;
            case 3: State[IndexToIncrement] = 5; Advanced = true; break;
            case 5: State[IndexToIncrement] = 7; Advanced = true; break;
            case 7: State[IndexToIncrement] = 9; Advanced = true; break;
            case 9: State[IndexToIncrement] = 0; break;
            InvalidDefaultCase;
        }
        if (Advanced) break;
        ++IndexToIncrement;
        if (IndexToIncrement == 6) break;
    }
    
    return Advanced;
}

internal void
Problem035()
{
    // 197 is a circular prime because rotating the digits always gives a prime (971 and 719)
    // How many circular primes are there below one million?
    // 2, 3, 5 and 7 are included.
    
    // for a prime number with at least two digits, in order to be a circular prime
    // it cannot have any 0, 2, 4, 5, 6 or 8. It can only have 1, 3, 7 and 9.
    u32 State[6] = {9, 0, 0, 0, 0, 0};
    u32 Result = 4;
    while (StateAdvance035(State))
    {
        u32 DigitCount = 6;
        while (State[DigitCount-1] == 0)
        {
            Assert(DigitCount > 1);
            --DigitCount;
        }
        
        b32 IsCircularPrime = true;
        for (u32 Offset = 0; Offset < DigitCount; ++Offset)
        {
            u32 RotatedNumber = BuildRotatedNumber(State, DigitCount, Offset);
            if (!IsPrime(RotatedNumber))
            {
                IsCircularPrime = false;
                break;
            }
        }
        if (IsCircularPrime)
            ++Result;
    }
    
    printf("%u\n", Result);
}

internal b32
IsBinaryPalindrome(u32 Number)
{
    b32 Result = true;
    
    u32 HighBitIndex = 31;
    u32 LowBitIndex = 0;
    
    while ((Number >> HighBitIndex) == 0)
    {
        HighBitIndex--;
    }
    
    while (HighBitIndex > LowBitIndex)
    {
        u32 HighBit = (Number >> HighBitIndex) & 1;
        u32 LowBit = (Number >> LowBitIndex) & 1;
        if (HighBit != LowBit)
        {
            Result = false;
            break;
        }
        --HighBitIndex;
        ++LowBitIndex;
    }
    
    return Result;
}
internal void
Problem036()
{
    // Sum of all numbers less than 1 million which are palindromic in both base 2 and 10.
    // Leading 0s don't count.
    u32 Sum = 0;
    
    Assert(IsBinaryPalindrome(585));
    
    for (u32 Low = 1; Low < 1000; Low+=2)
    {
        // NOTE(vincent): The least significant digit of a palindrome cannot be 0.
        // This applies for both decimal and binary palindromes.
        // And this is why we only study odd low parts.
        u32 High = 0;
        u32 PowerTen = 1;
        for (u32 CopyLow = Low; CopyLow > 0; CopyLow /= 10)
        {
            High = 10*High + CopyLow%10;
            PowerTen *= 10;
        }
        
        u32 BigDecimalPalindrome = PowerTen * High + Low;
        u32 TruncHigh = High / 10;
        u32 SmallDecimalPalindrome = PowerTen * TruncHigh + Low;
        
        if (IsBinaryPalindrome(BigDecimalPalindrome))
            Sum += BigDecimalPalindrome;
        if (IsBinaryPalindrome(SmallDecimalPalindrome))
            Sum += SmallDecimalPalindrome;
        
        // NOTE(vincent): If we stopped there we would miss palindromes like 909, 11011 etc.
        // where the Low part has leading zeroes.
        if (Low < 100)
        {
            u32 PaddedBigDecimalPalindrome = 10*PowerTen * High + Low;
            if (IsBinaryPalindrome(PaddedBigDecimalPalindrome))
                Sum += PaddedBigDecimalPalindrome;
            if (Low < 10)
            {
                u32 VeryPaddedBigDecimalPalindrome = 100*PowerTen*High + Low;
                if (IsBinaryPalindrome(VeryPaddedBigDecimalPalindrome))
                    Sum += VeryPaddedBigDecimalPalindrome;
            }
        }
        
    }
    printf("%u\n", Sum);
}

internal void
Problem037()
{
    // 3793 is a truncatable prime because 379, 37 and 3 are primes, and so are 793, 93 and 3.
    // Find the eleven truncatable primes, return the sum. 2, 3, 5 and 7 are not considered truncatable.
    
    // The first and last digits of a truncatable prime need to be primes themselves.
    // i.e.: 2, 3, 5 and 7.
    // The inner digits of a truncatable prime need to be odd and not 5.
    u32 Extremities[4] = {2,3,5,7};
    u32 InnerDigits[4] = {1,3,7,9};
    u32 Sum = 0;
    u32 FoundCount = 0;
    
    u32 BitLength = 4;
    u32 CandidateIndex = 0;
    while (CandidateIndex >> BitLength == 0)
    {
        u32 Digit1 = Extremities[(CandidateIndex >> 0) & 3];
        u32 Digit0 = Extremities[(CandidateIndex >> 2) & 3];
        u32 FullCandidate = 10*Digit1 + Digit0;
        if (IsPrime(10*Digit1 + Digit0))
        {
            Sum += FullCandidate;
            ++FoundCount;
        }
        ++CandidateIndex;
    }
    
    BitLength = 6;
    CandidateIndex = 0;
    while (CandidateIndex >> BitLength == 0)
    {
        u32 Digit2 = Extremities[(CandidateIndex >> 0) & 3];
        u32 Digit1 = InnerDigits[(CandidateIndex >> 2) & 3];
        u32 Digit0 = Extremities[(CandidateIndex >> 4) & 3];
        u32 FullCandidate = 100*Digit2 + 10*Digit1 + Digit0;
        if (IsPrime(FullCandidate) && IsPrime(10*Digit2 + Digit1) && IsPrime(10*Digit1 + Digit0))
        {
            Sum += FullCandidate;
            ++FoundCount;
        }
        ++CandidateIndex;
    }
    
    BitLength = 8;
    CandidateIndex = 0;
    while (CandidateIndex >> BitLength == 0)
    {
        u32 Digit3 = Extremities[(CandidateIndex >> 0) & 3];
        u32 Digit2 = InnerDigits[(CandidateIndex >> 2) & 3];
        u32 Digit1 = InnerDigits[(CandidateIndex >> 4) & 3];
        u32 Digit0 = Extremities[(CandidateIndex >> 6) & 3];
        u32 FullCandidate = 1000*Digit3 + 100*Digit2 + 10*Digit1 + Digit0;
        u32 a = 100*Digit2 + 10*Digit1 + Digit0;
        u32 b = 10*Digit1 + Digit0;
        u32 c = FullCandidate / 10;
        u32 d = FullCandidate / 100;
        if (IsPrime(FullCandidate) && IsPrime(a) && IsPrime(b) && IsPrime(c) && IsPrime(d))
        {
            Sum += FullCandidate;
            ++FoundCount;
        }
        ++CandidateIndex;
    }
    
    BitLength = 10;
    CandidateIndex = 0;
    while (CandidateIndex >> BitLength == 0)
    {
        u32 Digit4 = Extremities[(CandidateIndex >> 0) & 3];
        u32 Digit3 = InnerDigits[(CandidateIndex >> 2) & 3];
        u32 Digit2 = InnerDigits[(CandidateIndex >> 4) & 3];
        u32 Digit1 = InnerDigits[(CandidateIndex >> 6) & 3];
        u32 Digit0 = Extremities[(CandidateIndex >> 8) & 3];
        u32 FullCandidate = 10000*Digit4 + 1000*Digit3 + 100*Digit2 + 10*Digit1 + Digit0;
        u32 a = 1000*Digit3 + 100*Digit2 + 10*Digit1 + Digit0;
        u32 b = 100*Digit2 + 10*Digit1 + Digit0;
        u32 c = 10*Digit1 + Digit0;
        u32 d = FullCandidate / 10;
        u32 e = FullCandidate / 100;
        u32 f = FullCandidate / 1000;
        
        if (IsPrime(FullCandidate) && IsPrime(a) && IsPrime(b) && IsPrime(c) && IsPrime(d) && IsPrime(e)
            && IsPrime(f))
        {
            Sum += FullCandidate;
            ++FoundCount;
        }
        ++CandidateIndex;
    }
    
    BitLength = 12;
    CandidateIndex = 0;
    while (CandidateIndex >> BitLength == 0)
    {
        u32 Digit5 = Extremities[(CandidateIndex >> 0) & 3];
        u32 Digit4 = InnerDigits[(CandidateIndex >> 2) & 3];
        u32 Digit3 = InnerDigits[(CandidateIndex >> 4) & 3];
        u32 Digit2 = InnerDigits[(CandidateIndex >> 6) & 3];
        u32 Digit1 = InnerDigits[(CandidateIndex >> 8) & 3];
        u32 Digit0 = Extremities[(CandidateIndex >> 10) & 3];
        u32 FullCandidate = 100000*Digit5 + 10000*Digit4 + 1000*Digit3 + 100*Digit2 + 10*Digit1 + Digit0;
        u32 a = 10000*Digit4 + 1000*Digit3 + 100*Digit2 + 10*Digit1 + Digit0;
        u32 b = 1000*Digit3 + 100*Digit2 + 10*Digit1 + Digit0;
        u32 c = 100*Digit2 + 10*Digit1 + Digit0;
        u32 d = 10*Digit1 + Digit0;
        u32 e = FullCandidate / 10;
        u32 f = FullCandidate / 100;
        u32 g = FullCandidate / 1000;
        u32 h = FullCandidate / 10000;
        
        if (IsPrime(FullCandidate) && IsPrime(a) && IsPrime(b) && IsPrime(c) && IsPrime(d) && IsPrime(e)
            && IsPrime(f) && IsPrime(g) && IsPrime(h))
        {
            Sum += FullCandidate;
            ++FoundCount;
        }
        ++CandidateIndex;
    }
    
    Assert(FoundCount == 11);
    printf("%u\n", Sum);
}


internal u32
GetLargestU32ConcatenatedProduct(u32 Input)
{
    // NOTE(vincent): Return the largest concatenated product (defined in problem 38) 
    // that is 9 digits long at most.
    u32 Result = 0;
    u32 DigitCount = 0;
    u32 PowersOfTen[6] = {1, 10, 100, 1000, 10000, 100000};
    
    for (u32 n = 1; ; ++n)
    {
        u32 Append = Input*n;
        u32 AppendDigitCount = 0;
        for (u32 CopyAppend = Append; CopyAppend > 0; CopyAppend/=10)
        {
            ++AppendDigitCount;
        }
        
        if (DigitCount + AppendDigitCount <= 9)
        {
            Result = Result * PowersOfTen[AppendDigitCount] + Append;
            DigitCount += AppendDigitCount;
        }
        else
        {
            Assert(n > 1);
            break;
        }
    }
    Assert(Result < 1000 * 1000 * 1000);
    Assert(Result > Input);
    return Result;
}
internal void
Problem038InnerLoop(u32 Input, u32 *Result)
{
    u32 ConcatenatedProduct = GetLargestU32ConcatenatedProduct(Input);
    
    if (ConcatenatedProduct > *Result)
    {
        Assert(*Result >= 918273645);
        u32 DigitCount = 0;
        b32 FoundDigit[10] = {false};
        b32 IsPandigital = true;
        for (u32 CopyProduct = ConcatenatedProduct; CopyProduct > 0; CopyProduct /= 10)
        {
            u32 Digit = CopyProduct % 10;
            if (Digit == 0 || FoundDigit[Digit])
            {
                IsPandigital = false;
                break;
            }
            FoundDigit[Digit] = true;
            ++DigitCount;
        }
        
        if (DigitCount != 9)
            IsPandigital = false;
        if (IsPandigital)
        {
            *Result = ConcatenatedProduct;
        }
    }
}

internal void
Problem038()
{
    // 192384576 is the concatenated product of 192 and (1,2,3) because
    // 192*1 = 192, 192*2 = 384 and 192*3 = 576.
    // Likewise, 918273645 is the concatenated product of 9 and (1,2,3,4,5).
    // What's the largest 1 to 9 pandigital number that is the concatenated product of an integer with
    // (1,2,...,n) where n > 1?
    
    // NOTE(vincent): Input is at most 4 digits long.
    u32 Result = 918273645;
    
    for (u32 Input = 11; Input < 1000; ++Input)
        Problem038InnerLoop(Input, &Result);
    for (u32 Input = 9001; Input < 10000; ++Input)
        Problem038InnerLoop(Input, &Result);
    
    printf("%u\n", Result);
}

internal void
Problem039()
{
    // Find the integer p <= 1000 such that the number of right triangles of perimeter p
    // with integral sides a <= b <= c is maximized. 
    
    u32 BestCount = 0;
    u32 BestP = 0;
    for (u32 p = 3; p <= 1000; ++p)
    {
        u32 MaxA = p/3;
        u32 CountSolutions = 0;
        for (u32 a = 1; a < MaxA; ++a)
        {
            for (u32 b = a+1; 2*b < p - a; ++b) 
            {
                u32 c = p - b - a;
                Assert(c > b && b >= a);
                if (c*c == b*b + a*a)
                {
                    ++CountSolutions;
                }
            }
        }
        
        if (CountSolutions > BestCount)
        {
            BestCount = CountSolutions;
            BestP = p;
        }
    }
    
    printf("%u\n", BestP);
}

internal u32
ChampernowneDigit(u32 DigitIndex)
{
    // all fractional single digit numbers: 9 digits,   DigitIndex slice is [1,9] 
    // all the double digit numbers: (99 - 10 + 1) * 2 =  180 digits, slice [10,189] 
    // all the triple digit numbers: (999 - 99) * 3 = 2700 digits,    slice [190, 2889]
    // all the quad digit numbers: (9999 - 999) * 4 = 36000 digits,   slice [2890, 38889]
    // all the 5-digit numbers: (99999 - 9999) * 5 = 450000 digits,   slice [38890, 488889]
    
    u32 NumberSize, SliceBase, NumberBase;
    
    if (DigitIndex < 10)
    {
        NumberSize = 1;
        SliceBase = 1;
        NumberBase = 1;
    }
    else if (DigitIndex < 190)
    {
        NumberSize = 2;
        SliceBase = 10;
        NumberBase = 10;
    }
    else if (DigitIndex < 2890)
    {
        NumberSize = 3;
        SliceBase = 190;
        NumberBase = 100;
    }
    else if (DigitIndex < 38890)
    {
        NumberSize = 4;
        SliceBase = 2890;
        NumberBase = 1000;
    }
    else if (DigitIndex < 488890)
    {
        NumberSize = 5;
        SliceBase = 38890;
        NumberBase = 10000;
    }
    else
    {
        NumberSize = 6;
        SliceBase = 488890;
        NumberBase = 100000;
    }
    
    u32 DigitOffset = DigitIndex - SliceBase;
    u32 NumberOffset = DigitOffset / NumberSize;
    u32 Number = NumberOffset + NumberBase;
    u32 DigitOffsetInNumber = DigitOffset % NumberSize;
    u32 DivisionCount = (NumberSize - 1) - DigitOffsetInNumber;
    u32 CopyNumber = Number;
    for (u32 Div = 0; Div < DivisionCount; ++Div)
    {
        CopyNumber /= 10;
    }
    u32 Result = CopyNumber % 10;
    return Result;
}

internal void
Problem040()
{
    // Find the value of d1 * d10 * d100 * d1000 * d10000 * d100000 * d1000000
    // where d_n is the nth digit of the fractional part of Champernowne's constant.
    // .123456789 1011121314 1516171819 20...
    //  ^         ^
    
    Assert(ChampernowneDigit(1) == 1);
    Assert(ChampernowneDigit(2) == 2);
    Assert(ChampernowneDigit(12) == 1);
    Assert(ChampernowneDigit(11) == 0);
    Assert(ChampernowneDigit(10) == 1);
    Assert(ChampernowneDigit(9) == 9);
    
    u32 Product = 1;
    u32 Index = 1;
    for (u32 i = 0; i < 7; ++i)
    {
        Product *= ChampernowneDigit(Index);
        Index *= 10;
    }
    printf("%u\n", Product);
}

typedef struct
{
    u32 Size;
    u32 Choices[10];
}  permutation_choices;

internal void
InitPermutation(u32 Size, permutation_choices *Permutation)
{
    Assert(Size <= ArrayCount(Permutation->Choices));
    Permutation->Size = Size;
    for (u32 Index = 0; Index < Size; ++Index)
        Permutation->Choices[Index] = 0;
}

internal b32
AdvancePermutation(permutation_choices *Permutation)
{
    b32 Advanced = false;
    u32 Size = Permutation->Size;
    
    for (u32 ChangingIndex = 0; ChangingIndex < Size-1; ++ChangingIndex)
    {
        if (Permutation->Choices[ChangingIndex] < Size-1 - ChangingIndex)
        {
            Permutation->Choices[ChangingIndex]++;
            Advanced = true;
            break;
        }
        else
        {
            Permutation->Choices[ChangingIndex] = 0;
        }
    }
    
    return Advanced;
}

internal void
SetBit(u32 *Bits, u32 Index)
{
    *Bits |= (1 << Index);
}

internal b32
GetBit(u32 Bits, u32 Index)
{
    b32 Result = (Bits >> Index) & 1;
    return Result;
}

internal void
WritePermutationArrayFromChoices(permutation_choices *Choices, u32 *Dest, u32 ValueOffset)
{
    u32 Size = Choices->Size;
    u32 Bits = 0;
    
    for (u32 ChoiceIndex = 0; ChoiceIndex < Size; ++ChoiceIndex)
    {
        u32 ChoiceValue = Choices->Choices[ChoiceIndex];
        u32 UnsetBitCount = 0;
        u32 BitIndex = 0;
        for (;;)
        {
            if (GetBit(Bits, BitIndex) == 0)
            {
                ++UnsetBitCount;
                if (UnsetBitCount > ChoiceValue)
                    break;
            }
            ++BitIndex;
        }
        SetBit(&Bits, BitIndex);
        Dest[ChoiceIndex] = BitIndex + ValueOffset; // NOTE(vincent): offset by 1 for 1-based values
    }
}

internal void
Problem041()
{
    // Find the largest 1 to n pandigital prime number.
    
    // Suppose you are building a permutation - a sequence where digits from 1 to n appear exactly once.
    // There are n possibilities for the first digit of the sequence.
    // Second digit: n-1 possibilities (eliminated previous choice)
    // ...etc.
    
    permutation_choices Permutation;
    u32 PermArray[10];
    
    u32 Result = 0;
    
    for (u32 DigitCount = 9; Result == 0 ; --DigitCount)
    {
        InitPermutation(DigitCount, &Permutation);
        do
        {
            WritePermutationArrayFromChoices(&Permutation, PermArray, 1);
            u32 DecimalNumber = 0;
            for (u32 DigitIndex = 0; DigitIndex < DigitCount; ++DigitIndex)
            {
                DecimalNumber = 10*DecimalNumber + PermArray[DigitIndex];
            }
            if (IsPrime(DecimalNumber) && DecimalNumber > Result)
            {
                Result = DecimalNumber;
            }
        } while(AdvancePermutation(&Permutation));
        
        if (Result)
            break;
    }
    
    printf("%u\n", Result);
}

typedef struct 
{
    u32 Size;
    u32 *Bits;
} bit_table;

internal void
TableSetBit(bit_table *Table, u32 Index)
{
    u32 BucketIndex = Index >> 5;
    u32 BucketOffset = Index & 31;
    Table->Bits[BucketIndex] |= 1 << BucketOffset;
}

internal b32
TableGetBit(bit_table *Table, u32 Index)
{
    u32 BucketIndex = Index >> 5;
    u32 BucketOffset = Index & 31;
    b32 Result = (Table->Bits[BucketIndex] >> BucketOffset) & 1;
    return Result;
}

internal void
Problem042(memory_arena *Arena)
{
    temporary_memory TempMemory = BeginTemporaryMemory(Arena);
    push_read_file ReadFile = PushReadFile(Arena, "text042");
    Assert(ReadFile.Success && ReadFile.Memory && ReadFile.Size > 0);
    char *At = (char *)ReadFile.Memory;
    char *End = At + ReadFile.Size;
    
    bit_table TrianglesLookup;
    TrianglesLookup.Size = 512;
    TrianglesLookup.Bits = PushSize(Arena, TrianglesLookup.Size / 8);
    for (u32 Word = 0; Word < 512 / 32; ++Word)
        TrianglesLookup.Bits[Word] = 0;
    u32 Triangle = 1;
    Assert(!TableGetBit(&TrianglesLookup, 17));
    for (u32 Index = 2; Triangle < TrianglesLookup.Size; ++Index)
    {
        TableSetBit(&TrianglesLookup, Triangle);
        Assert(!TableGetBit(&TrianglesLookup, 17));
        Triangle += Index;
    }
    
    Assert(TableGetBit(&TrianglesLookup, 1));
    Assert(TableGetBit(&TrianglesLookup, 3));
    Assert(TableGetBit(&TrianglesLookup, 6));
    Assert(TableGetBit(&TrianglesLookup, 10));
    Assert(TableGetBit(&TrianglesLookup, 15));
    Assert(TableGetBit(&TrianglesLookup, 21));
    Assert(TableGetBit(&TrianglesLookup, 28));
    Assert(TableGetBit(&TrianglesLookup, 36));
    Assert(TableGetBit(&TrianglesLookup, 45));
    Assert(!TableGetBit(&TrianglesLookup, 2));
    Assert(!TableGetBit(&TrianglesLookup, 4));
    Assert(!TableGetBit(&TrianglesLookup, 5));
    Assert(!TableGetBit(&TrianglesLookup, 7));
    Assert(!TableGetBit(&TrianglesLookup, 8));
    Assert(!TableGetBit(&TrianglesLookup, 9));
    Assert(!TableGetBit(&TrianglesLookup, 11));
    Assert(!TableGetBit(&TrianglesLookup, 12));
    Assert(!TableGetBit(&TrianglesLookup, 13));
    Assert(!TableGetBit(&TrianglesLookup, 14));
    Assert(!TableGetBit(&TrianglesLookup, 16));
    Assert(!TableGetBit(&TrianglesLookup, 17));
    Assert(!TableGetBit(&TrianglesLookup, 18));
    Assert(!TableGetBit(&TrianglesLookup, 19));
    Assert(!TableGetBit(&TrianglesLookup, 20));
    Assert(!TableGetBit(&TrianglesLookup, 22));
    Assert(!TableGetBit(&TrianglesLookup, 23));
    Assert(!TableGetBit(&TrianglesLookup, 24));
    Assert(!TableGetBit(&TrianglesLookup, 25));
    Assert(!TableGetBit(&TrianglesLookup, 26));
    Assert(!TableGetBit(&TrianglesLookup, 27));
    Assert(!TableGetBit(&TrianglesLookup, 29));
    Assert(!TableGetBit(&TrianglesLookup, 30));
    Assert(!TableGetBit(&TrianglesLookup, 31));
    Assert(!TableGetBit(&TrianglesLookup, 32));
    Assert(!TableGetBit(&TrianglesLookup, 33));
    Assert(!TableGetBit(&TrianglesLookup, 34));
    Assert(!TableGetBit(&TrianglesLookup, 35));
    Assert(!TableGetBit(&TrianglesLookup, 37));
    
    
    u32 TriangleWordsCount = 0;
    u32 CurrentWordValue = 0;
    while (At < End)
    {
        if ('A' <= *At && *At <= 'Z')
        {
            u32 CharValue = *At - 'A' + 1;
            CurrentWordValue += CharValue;
        }
        else
        {
            if (CurrentWordValue > 0 && TableGetBit(&TrianglesLookup, CurrentWordValue))
            {
                ++TriangleWordsCount;
            }
            CurrentWordValue = 0;
        }
        ++At;
    }
    
    EndTemporaryMemory(TempMemory);
    printf("%u\n", TriangleWordsCount);
}

internal void
Problem043()
{
    // Let dn refer to the nth digit of a number.
    // Find the sum of all 0 to 9 pandigital numbers with the following substring properties:
    // d2d3d4 is divisible by 2
    // d3d4d5 is divisible by 3
    // d4d5d6 is divisible by 5
    // d5d6d7 is divisible by 7
    // d6d7d8 is divisible by 11
    // d7d8d9 is divisible by 13
    // d8d9d10 is divisible by 17
    
    permutation_choices Permutation;
    u32 DigitCount = 10;
    InitPermutation(DigitCount, &Permutation);
    // TODO(vincent): Might be able to work from 9! perms instead of 10!,
    // seeing that d1 is not involved in the substring divisibility constraint.
    u64 Sum = 0;
    u32 PermDigits[10];
    do
    {
        // NOTE(vincent): skip when "most significant" digit is 0.
        if (Permutation.Choices[0] == 0)
            continue;
        
        WritePermutationArrayFromChoices(&Permutation, PermDigits, 0);
        
        u32 SubNumber6 = 100*PermDigits[7] + 10*PermDigits[8] + PermDigits[9];
        if (SubNumber6 % 17) continue;
        u32 SubNumber5 = 100*PermDigits[6] + 10*PermDigits[7] + PermDigits[8];
        if (SubNumber5 % 13) continue;
        u32 SubNumber4 = 100*PermDigits[5] + 10*PermDigits[6] + PermDigits[7];
        if (SubNumber4 % 11) continue;
        u32 SubNumber3 = 100*PermDigits[4] + 10*PermDigits[5] + PermDigits[6];
        if (SubNumber3 % 7) continue;
        u32 SubNumber2 = 100*PermDigits[3] + 10*PermDigits[4] + PermDigits[5];
        if (SubNumber2 % 5) continue;
        u32 SubNumber1 = 100*PermDigits[2] + 10*PermDigits[3] + PermDigits[4];
        if (SubNumber1 % 3) continue;
        u32 SubNumber0 = 100*PermDigits[1] + 10*PermDigits[2] + PermDigits[3];
        if (SubNumber0 & 1) continue;
        
        u64 Number = 0;
        for (u32 DigitIndex = 0; DigitIndex < DigitCount; ++DigitIndex)
        {
            Number = 10*Number + PermDigits[DigitIndex];
        }
        Sum += Number;
        
    } while(AdvancePermutation(&Permutation));
    
    printf("%llu\n", Sum);
}

internal b32
BinarySearch(u32 *Array, u32 Count, u32 Number)
{
    s32 MinIndex = 0;
    s32 MaxIndex = Count > 0 ? Count - 1 : 0;
    
    b32 Result = false;
    while (MinIndex <= MaxIndex)
    {
        s32 MidIndex = (MinIndex + MaxIndex) >> 1;
        if (Array[MidIndex] == Number)
        {
            Result = true;
            break;
        }
        if (Number > Array[MidIndex])
            MinIndex = MidIndex + 1;
        else
            MaxIndex = MidIndex - 1;
    }
    return Result;
}

internal void
Problem044(memory_arena *Arena)
{
    // Pentagonal numbers: Pn = n(3n-1)/2. Starts as 1, 5, 12, 22, 35, etc.
    // Find the pair of pentagonal numbers for which their sum and difference are pentagonal,
    // and the difference D is minimal. What is the value of D?
    
    // P(n+a) - P(n) = P(b) = D = (1/2)a(a+6n-1)
    // P(n+a) + P(n) = P(c)
    // 2P(n+a) = P(c) + P(b)
    // 2P(n)   = P(c) - P(b) = n(3n-1)
    
    // search for (b, c) in this order: 
    // (1,2) (1,3) (1,4) ... (1, something large)
    // (2,3) (2,4) (2,5) ... (2, something large)
    // etc.
    
    temporary_memory PentagonsMemory = BeginTemporaryMemory(Arena);
    u32 *Pentagons = (u32 *)(Arena->Base + Arena->Used);
    u32 PentagonsCount = 0;
    while (PentagonsCount < 2400) // Can set the smallest value yielding Result > 0
    {
        u32 Pentagon = ((PentagonsCount+1) * (3*(PentagonsCount+1)-1)) >> 1;
        *PushStruct(Arena, u32) = Pentagon;
        ++PentagonsCount;
    }
    
    u32 Result = 0;
    for (u32 b = 1;; ++b)
    {
        for (u32 c = b + 1; c < PentagonsCount; ++c)
        {
            u32 CmBd2 = (Pentagons[c] - Pentagons[b]) >> 1;
            u32 CpBd2 = (Pentagons[c] + Pentagons[b]) >> 1;
            b32 IsPentagon1 = BinarySearch(Pentagons, PentagonsCount, CmBd2);
            if (IsPentagon1)
            {
                b32 IsPentagon2 = BinarySearch(Pentagons, PentagonsCount, CpBd2);
                if (IsPentagon2)
                {
                    //printf("b: %u\n", c);
                    Result = Pentagons[b];
                    goto Label_Done044;
                }
            }
        }
    }
    
    Label_Done044:
    
    EndTemporaryMemory(PentagonsMemory);
    printf("%u\n", Result);
}

internal void
DrawModTableLinear(u32 a, u32 b)
{
    printf("an + b mod table for a = %u b = %u\n", a, b);
    printf("n:      ");
    for (u32 n = 0; n < 10; ++n)
    {
        printf("%5u", n);
    }
    for (u32 Mod = 2; Mod < 9; ++Mod)
    {
        printf("mod %u:  ", Mod);
        for (u32 n = 0; n < 10; ++n)
        {
            printf("%5u", (a*n+b)%Mod);
        }
        printf("\n");
    }
}


internal void
DrawModTableQuadratic(s32 a, s32 b, s32 c)
{
    printf("an^2 + bn + c mod table for a = %d b = %d c = %d:\n", a, b, c);
    printf("n:       ");
    for (u32 n = 0; n < 10; ++n)
    {
        printf("%5u", n);
    }
    printf("\n");
    for (u32 Mod = 2; Mod < 28; ++Mod)
    {
        printf("mod %2u:  ", Mod);
        for (u32 n = 0; n < Mod; ++n)
        {
            printf("%5u", (a*n*n + b*n + c)%Mod);
        }
        printf("\n");
    }
}

internal void
Problem045()
{
    // Tn = n(n+1)/2
    // Pn = n(3n-1)/2
    // Hn = n(2n-1)
    // T285 = P165 = H143 = 40755
    // find the next triangle number that is also pentagonal and hexagonal.
    u32 IndexHex = 144;
    u32 IndexTri = 286;
    u32 IndexPen = 166;
    
    u64 Hex = (IndexHex * (2*IndexHex-1));
    u64 Pen = (IndexPen * (3*IndexPen-1)) >> 1;
    u64 Tri = (IndexTri * (IndexTri+1)) >> 1;
    
    for (;;)
    {
        while (Tri < Hex || Tri < Pen)
        {
            ++IndexTri;
            Tri += IndexTri;
        }
        while (Hex < Pen || Hex < Tri)
        {
            Hex += 4*IndexHex+1;
            ++IndexHex;
        }
        while (Pen < Hex || Pen < Tri)
        {
            Pen += 3*IndexPen+1;
            ++IndexPen;
        }
        if (Pen == Hex && Hex == Tri)
            break;
    }
    printf("%llu\n", Hex);
}

internal b32
IsSquare(u32 N)
{
    u32 MinRootCandidate = 1;
    u32 MaxRootCandidate = (N < 65536 ? N : 65535);
    b32 Result = false;
    while (MinRootCandidate <= MaxRootCandidate)
    {
        u32 RootCandidate = (MinRootCandidate + MaxRootCandidate) >> 1;
        u32 MaybeSquare = RootCandidate * RootCandidate;
        if (MaybeSquare < N)
        {
            MinRootCandidate = RootCandidate + 1;
        }
        else if (MaybeSquare > N)
        {
            MaxRootCandidate = RootCandidate - 1;
        }
        else 
        {
            Result = true;
            break;
        }
    }
    return Result;
}

internal void
Problem046(memory_arena *Arena)
{
    // What is the smallest odd composite that cannot be written 
    // as the sum of a prime and twice a square?
    temporary_memory PrimesMem = BeginTemporaryMemory(Arena);
    
    u32 *Primes = PushArray(Arena, 4, u32);
    Primes[0] = 2;
    Primes[1] = 3;
    Primes[2] = 5;
    Primes[3] = 7;
    u32 Result = 7;
    u32 MemorizedPrimesCount = 4;
    
    
    for (u32 SquareRoot = 1; SquareRoot < 1000; ++SquareRoot)
    {
        Assert(IsSquare(SquareRoot * SquareRoot));
        Assert(!IsSquare(SquareRoot * SquareRoot + 1));
        Assert(!IsSquare(SquareRoot * SquareRoot + 2));
    }
    
    b32 Found = false;
    while (!Found)
    {
        Result += 2;
        if (!IsPrime(Result))
        {
            Found = true;
            for (u32 PrimeIndex = 0; PrimeIndex < MemorizedPrimesCount; ++PrimeIndex)
            {
                u32 Subtraction = Result - Primes[PrimeIndex];
                u32 SquareCandidate = Subtraction >> 1;
                if (IsSquare(SquareCandidate))
                {
                    Found = false;
                    break;
                }
            }
        }
        else
        {
            *PushStruct(Arena, u32) = Result;
            ++MemorizedPrimesCount;
        }
        
    }
    
    EndTemporaryMemory(PrimesMem);
    printf("%u\n", Result);
}



internal void
Problem047(memory_arena *Arena)
{
    // Find the first four consecutive integers to have four distinct prime factors each.
    // What is the first of these numbers?
    
    temporary_memory PrimesMem = BeginTemporaryMemory(Arena);
    u32 *Primes = PushArray(Arena, 5, u32);
    Primes[0] = 2;
    Primes[1] = 3;
    Primes[2] = 5;
    Primes[3] = 7;
    Primes[4] = 11;
    u32 PrimesCount = 5;
    
    u32 InARow = 0;
    u32 Result = 0;
    for (u32 Number = 12; Result == 0;++Number)
    {
        u32 CopyNumber = Number;
        u32 PrimeFactorsCount = 0;
        for (u32 PrimeIndex = 0; PrimeIndex < PrimesCount; ++PrimeIndex)
        {
            u32 Prime = Primes[PrimeIndex];
            if (CopyNumber % Prime == 0)
            {
                ++PrimeFactorsCount;
                do
                {
                    CopyNumber /= Prime;
                } while (CopyNumber % Prime == 0);
                if (CopyNumber < Prime)
                    break;
            }
        }
        
        if (CopyNumber == Number)
        {
            Assert(PrimeFactorsCount == 0);
            *PushStruct(Arena, u32) = Number;
            ++PrimesCount;
        }
        if (PrimeFactorsCount == 4)
        {
            InARow++;
            if (InARow == 4)
            {
                Result = Number - 3;
            }
        }
        else
            InARow = 0;
    }
    EndTemporaryMemory(PrimesMem);
    printf("%u\n", Result);
}

internal void
Problem048()
{
    // Last ten digits of 1^1 + 2^2 + 3^3 + ... + 1000^1000.
    u64 Result = 0;
    u64 Modulo = 1000ull * 1000ull * 1000ull * 10ull;
    for (u64 N = 1; N <= 1000; ++N)
    {
        u64 Term = 1;
        for (u32 k = 0; k < N; ++k)
        {
            Term = (Term*N) % Modulo;
        }
        Result = (Result + Term) % Modulo;
    }
    
    printf("%llu\n", Result);
}

internal void
Problem049()
{
    // The arithmetic sequence 1487, 4817, 8147 is made of three primes and each of the 4-digit numbers
    // are permutations of one another.
    // Find the other 4-digit arithmetic sequence which has this property, concatenate.
    
    u64 Result = 0;
#define FILL_DIGITS_ARRAY(Digits, Number) \
Digits[Number % 10]         += 1 ;\
Digits[(Number / 10) % 10]  += 1 ;\
Digits[(Number / 100) % 10] += 1 ;\
Digits[Number / 1000]       += 1 ;
    
    
    for (u32 StartingNumber = 1001; Result == 0 && StartingNumber < 9999; StartingNumber += 2)
    {
        u32 Digits[10] = {0};
        FILL_DIGITS_ARRAY(Digits, StartingNumber);
        
        if (!IsPrime(StartingNumber))
            continue;
        
        for (u32 Increment = 4; StartingNumber + 2*Increment < 10000; Increment += 2)
        {
            if (StartingNumber == 1487 && Increment == 3330)
                continue;
            u32 Number2 = StartingNumber + Increment;
            u32 Number3 = StartingNumber + 2*Increment;
            u32 Digits2[10] = {0};
            u32 Digits3[10] = {0};
            FILL_DIGITS_ARRAY(Digits2, Number2);
            FILL_DIGITS_ARRAY(Digits3, Number3);
            
#undef FILL_DIGITS_ARRAY
            
            b32 ArePermutations = true;
            for (u32 Index = 0; Index < ArrayCount(Digits2); ++Index)
            {
                if (Digits[Index] != Digits2[Index] || Digits2[Index] != Digits3[Index])
                {
                    ArePermutations = false;
                    break;
                }
            }
            
            if (ArePermutations)
            {
                if (IsPrime(Number2) && IsPrime(Number3))
                {
                    Result = StartingNumber * 10000llu * 10000llu + Number2 * 10000llu + Number3;
                    break;
                }
            }
        }
    }
    printf("%llu\n", Result);
}

internal void
Problem050(memory_arena *Arena)
{
    // Which prime, below one million, can be written as the sum of the most consecutive primes?
    // 41 = 2 + 3 + 5 + 7 + 11 + 13 so you're looking for at least 7 terms.
    // The smallest term is < 1,000,000 / 7 = 142,857.142... 
    
    temporary_memory PrimesMemory = BeginTemporaryMemory(Arena);
    
    u32 *Primes = PushArray(Arena, 5, u32);
    Primes[0] = 2;
    Primes[1] = 3;
    Primes[2] = 5;
    Primes[3] = 7;
    Primes[4] = 11;
    u32 PrimesCount = 5;
    for (u32 PrimeCandidate = 13; PrimeCandidate < 1000 * 1000; PrimeCandidate += 2)
    {
        if (IsPrime(PrimeCandidate))
        {
            *PushStruct(Arena, u32) = PrimeCandidate;
            ++PrimesCount;
        }
    }
    
    u32 Result = 0;
    u32 BestTermCount = 6;
    for (u32 StartIndex = 0; StartIndex < PrimesCount - 6; ++StartIndex)
    {
        u32 TermCount = 6;
        u32 Sum = Primes[StartIndex] + Primes[StartIndex+1] + Primes[StartIndex+2] +
            Primes[StartIndex+3] + Primes[StartIndex+4] + Primes[StartIndex+5];
        u32 BestPrime = 0;
        u32 NextIndexToAdd = StartIndex+6;
        Assert(NextIndexToAdd < PrimesCount);
        while (Sum < 1000 * 1000)
        {
            Sum += Primes[NextIndexToAdd];
            ++TermCount;
            ++NextIndexToAdd;
            if (TermCount > BestTermCount && Sum < 1000*1000 && IsPrime(Sum))
            {
                BestTermCount = TermCount;
                Result = Sum;
            }
        }
    }
    EndTemporaryMemory(PrimesMemory);
    printf("%u\n", Result);
}

typedef struct
{
    u32 ChooseCount;
    u32 SlotsCount;
    u32 Choices[10];
} arrangement_choices;

internal void
InitArrangement(u32 ChooseCount, u32 SlotsCount, arrangement_choices *Combination)
{
    Assert(ChooseCount <= SlotsCount && SlotsCount < ArrayCount(Combination->Choices));
    Combination->ChooseCount = ChooseCount;
    Combination->SlotsCount = SlotsCount;
    for (u32 Index = 0; Index < ChooseCount; ++Index)
        Combination->Choices[Index] = 0;
}

internal b32
AdvanceArrangement(arrangement_choices *Combination)
{
    // TODO(vincent): This is buggy. (Not changing that last choice?)
    b32 Advanced = false;
    u32 ChooseCount = Combination->ChooseCount;
    u32 SlotsCount = Combination->SlotsCount;
    
    for (u32 ChangingIndex = 0; ChangingIndex < ChooseCount - 1; ++ChangingIndex)
    {
        if (Combination->Choices[ChangingIndex] < SlotsCount - 1 - ChangingIndex)
        {
            Combination->Choices[ChangingIndex]++;
            Advanced = true;
            break;
        }
        else
        {
            Combination->Choices[ChangingIndex] = 0;
        }
    }
    return Advanced;
}

internal void
WriteArrangementArrayFromChoices(arrangement_choices *Choices, u32 *Dest, u32 ValueOffset)
{
    u32 SlotsCount = Choices->SlotsCount;
    u32 Bits = 0;
    for (u32 ChoiceIndex = 0; ChoiceIndex < SlotsCount; ++ChoiceIndex)
    {
        u32 ChoiceValue = Choices->Choices[ChoiceIndex];
        u32 UnsetBitCount = 0;
        u32 BitIndex = 0;
        for (;;)
        {
            if (GetBit(Bits, BitIndex) == 0)
            {
                ++UnsetBitCount;
                if (UnsetBitCount > ChoiceValue)
                    break;
            }
            ++BitIndex;
        }
        SetBit(&Bits, BitIndex);
        Dest[ChoiceIndex] = BitIndex + ValueOffset;
    }
}

internal b32
Problem052StateAdvance(u32 *State)
{
    b32 Advanced = false;
    u32 LSD = State[0];
    switch(LSD)
    {
        case 1: State[0] = 3; Advanced = true; break;
        case 3: State[0] = 5; Advanced = true; break;
        case 5: State[0] = 7; Advanced = true; break;
        case 7: State[0] = 1; break;
        InvalidDefaultCase;
    }
    
    if (!Advanced)
    {
        for (u32 Index = 1; Index < 6 && !Advanced; ++Index)
        {
            if (State[Index] < 10)
            {
                State[Index]++;
                Advanced = true;
            }
            else
            {
                State[Index] = 0;
            }
        }
    }
    return Advanced;
}

internal void
Problem051()
{
    // find the smallest prime which, by replacing part of the number (not necessarily adjacent digits)
    // with the same digit, is part of an eight prime value family.
    
    // e.g. 13, 23, 43, 53, 73, and 83  forms a six prime family, out of the nine possible value from
    // substituting the first digit. (Looks like you can't substitute the first digit with a zero, 
    // whereas inner digits are fine)
    
    // Answer must be at least two digits long.
    // Last digit can't be a * (joker). There are only four possible last digits for 
    
    u32 Result = 0;
    u32 State[6] = {1, 1, 0, 0, 0, 0};
    
    //                 *  *     *   
    //              0  1  1  0  1  0
    while (!Result)
    {
        u32 Number = 0;
        u32 JokerFactor = 0;
        for (s32 StateIndex = ArrayCount(State) - 1; StateIndex >= 0; --StateIndex)
        {
            Number *= 10;
            JokerFactor *= 10;
            u32 StateElement = State[StateIndex];
            if (StateElement == 10)
                JokerFactor += 1;
            else
                Number += StateElement;
        }
        
        if (JokerFactor)
        {
            u32 PrimeFamily[8];
            u32 FamilyCount = 0;
            
            s32 IndexMostSignificantDigit = ArrayCount(State) - 1;
            while (State[IndexMostSignificantDigit] == 0)
            {
                --IndexMostSignificantDigit;
                Assert(IndexMostSignificantDigit >= 0);
            }
            
            b32 MostSignificantDigitIsJoker = State[IndexMostSignificantDigit] == 10;
            u32 JokerDigitStartingValue = MostSignificantDigitIsJoker ? 1 : 0;
            for (u32 JokerDigit = JokerDigitStartingValue; JokerDigit < 10; ++JokerDigit)
            {
                u32 PrimeCandidate = Number + JokerDigit*JokerFactor;
                if (IsPrime(PrimeCandidate))
                {
                    PrimeFamily[FamilyCount] = PrimeCandidate;
                    ++FamilyCount;
                    if (FamilyCount == 8)
                    {
                        Result = PrimeFamily[0];
                    }
                }
            }
        }
        
        Problem052StateAdvance(State);
        
    }
    
    printf("%u\n", Result);
    
    
}


internal void
Problem052()
{
    // Smallest positive integer x such that x, 2x, 3x, 4x, 5x and 6x contain the same digits.
    
    u32 PowerTen[7] = {1, 10, 100, 1000, 10000, 100000, 1000*1000};
    u32 CurrentPowerTen = 0;
    u32 Result = 0;
    for (u32 x = 1;; )
    {
        u32 DigitsX[10] = {0};
        for (u32 CopyX = x; CopyX > 0; CopyX /= 10)
            DigitsX[CopyX % 10]++;
        
        b32 Yes = true;
        for (u32 Factor = 2; Yes && Factor < 7; ++Factor)
        {
            u32 Multiple = Factor * x;
            u32 DigitsMultiple[10] = {0};
            for (u32 CopyMultiple = Multiple; CopyMultiple > 0; CopyMultiple /= 10)
            {
                u32 Digit = CopyMultiple % 10;
                if (DigitsMultiple[Digit] >= DigitsX[Digit])
                {
                    Yes = false;
                    break;
                }
                DigitsMultiple[Digit]++;
            }
        }
        
        if (Yes)
        {
            Result = x;
            break;
        }
        
        ++x;
        if (6*x > PowerTen[CurrentPowerTen+1])
        {
            ++CurrentPowerTen;
            x = PowerTen[CurrentPowerTen];
        }
    }
    
    printf("%u\n", Result);
}

internal void
Problem053(memory_arena *Arena)
{
    // How many distinct values of (n choose r) for 1 <= n <= 100 are greater than one million?
    
    
    // Pascal's triangle formula: C(n+1, r+1) = C(n, r+1) + C(n, r).
    
    // (0, 0)  initialized to 1 but doesn't count for the result
    // (1, 0) (2, 0) ... (100, 0)  100 terms all initialized to 1
    // (1, 1) (2, 1) ... (100, 1)  100 terms, first one initialized to 1, 
    // otherwise sum 1st and 101st previous terms.
    // (2, 2) (3, 2) ... (100, 2) 99 terms, first one initialized to 1, else sum 1st and 100th previous.
    
    // (100, 99)  = (99, 99) + (99, 98) -->  1st and 3rd previous terms when r = 99.
    // pushing order is: (99, 98), (100, 98), (99, 99), (100, 99) 
    
    u32 Result = 0;
    temporary_memory BinomialMemory = BeginTemporaryMemory(Arena);
    
    for (s32 r = 0; r <= 100; ++r)
        *PushStruct(Arena, u32) = 1;
    
    for (s32 r = 1; r <= 100; ++r)
    {
        *PushStruct(Arena, u32) = 1;
        for (s32 n = r+1; n <= 100; ++n)
        {
            u32 *Current = PushStruct(Arena, u32);
            u32 Sum = Current[-1] + Current[r-102];
            if (Sum > 1000 * 1000)
            {
                ++Result;
                Sum = 1000 * 1000 + 1;
            }
            *Current = Sum;
            //printf("n: %u r: %u val: %u\n", n, r, *Current);
        }
    }
    
    EndTemporaryMemory(BinomialMemory);
    printf("%u\n", Result);
}


typedef enum
{
    HandRank_HighCard,
    HandRank_OnePair,
    HandRank_TwoPairs,
    HandRank_ThreeOfAKind,
    HandRank_Straight,
    HandRank_Flush,
    HandRank_FullHouse,
    HandRank_FourOfAKind,
    HandRank_StraightFlush,
    HandRank_RoyalFlush,
} hand_rank;

typedef struct
{
    u32 Value;  // 2, 3, 4, 5, 6, 7, 8, 9, 10, Jack, Queen, King, Ace.
    u32 Suit;    // Diamonds, Hearts, Clubs, Spades
} card;

typedef struct
{
    hand_rank Rank;
    u32 HighPairValue;
    u32 LowPairValue;
    u32 TripleValue;
    u32 QuadValue;
} hand_info;

internal void
SortAndGetHandInfo(card *Cards, hand_info *Info)
{
    // NOTE(vincent): Sorting cards from highest value to lowest value.
    for (u32 InsertionIndex = 0; InsertionIndex < 5; ++InsertionIndex)
    {
        u32 BestValue = Cards[InsertionIndex].Value;
        for (u32 SourceIndex = InsertionIndex + 1; SourceIndex < 5; ++SourceIndex)
        {
            u32 SourceValue = Cards[SourceIndex].Value;
            if (SourceValue > BestValue)
            {
                BestValue = SourceValue;
                card Temp = Cards[InsertionIndex];
                Cards[InsertionIndex] = Cards[SourceIndex];
                Cards[SourceIndex] = Temp;
            }
        }
    }
    
    Assert(Cards[0].Value >= Cards[1].Value &&
           Cards[1].Value >= Cards[2].Value &&
           Cards[2].Value >= Cards[3].Value &&
           Cards[3].Value >= Cards[4].Value);
    
    b32 ConsecutiveValues = (Cards[0].Value == Cards[1].Value + 1 && 
                             Cards[1].Value == Cards[2].Value + 1 &&
                             Cards[2].Value == Cards[3].Value + 1 &&
                             Cards[3].Value == Cards[4].Value + 1);
    b32 SameSuit = (Cards[0].Suit == Cards[1].Suit &&
                    Cards[1].Suit == Cards[2].Suit &&
                    Cards[2].Suit == Cards[3].Suit &&
                    Cards[3].Suit == Cards[4].Suit);
    
    u32 PairCount = 0;
    b32 HasTriple = false;
    b32 HasQuad = false;
    
    Info->HighPairValue = 0;
    Info->LowPairValue = 0;
    Info->TripleValue = 0;
    Info->QuadValue = 0;
    
    u32 CurrentValue = Cards[0].Value;
    u32 OccurrenceCount = 1;
    for (u32 CardIndex = 1; CardIndex < 5; ++CardIndex)
    {
        u32 Value = Cards[CardIndex].Value;
        if (Value == CurrentValue)
            OccurrenceCount++;
        if (Value != CurrentValue || CardIndex == 4)
        {
            switch (OccurrenceCount)
            {
                case 1: break;
                case 2: 
                ++PairCount; 
                if (Info->HighPairValue)
                {
                    Assert(Info->HighPairValue >= CurrentValue);
                    Info->LowPairValue = CurrentValue;
                }
                else
                    Info->HighPairValue = CurrentValue;
                break;
                case 3: HasTriple = true; Info->TripleValue = CurrentValue; break;
                case 4: HasQuad = true;   Info->QuadValue = CurrentValue;  break;
                InvalidDefaultCase;
            }
            OccurrenceCount = 1;
            CurrentValue = Value;
        }
    }
    
    Info->Rank = HandRank_HighCard;
    
    if (ConsecutiveValues && Cards[0].Value == 14 && SameSuit)
        Info->Rank = HandRank_RoyalFlush;
    else if (ConsecutiveValues && SameSuit)
        Info->Rank = HandRank_StraightFlush;
    else if (HasQuad)
        Info->Rank = HandRank_FourOfAKind;
    else if (HasTriple && PairCount == 1)
        Info->Rank = HandRank_FullHouse;
    else if (SameSuit)
        Info->Rank = HandRank_Flush;
    else if (ConsecutiveValues)
        Info->Rank = HandRank_Straight;
    else if (HasTriple)
        Info->Rank = HandRank_ThreeOfAKind;
    else if (PairCount == 2)
        Info->Rank = HandRank_TwoPairs;
    else if (PairCount == 1)
        Info->Rank = HandRank_OnePair;
    
}

internal void
Problem054(memory_arena *Arena)
{
    // Evaluate poker hands, find how many times player 1 wins.
    
    card Cards[10];
    u32 LaidDownCount = 0;
    u32 Result = 0;
    temporary_memory TempMemory = BeginTemporaryMemory(Arena);
    push_read_file ReadFile = PushReadFile(Arena, "text054");
    Assert(ReadFile.Success && ReadFile.Memory && ReadFile.Size > 0);
    char *At = (char *)ReadFile.Memory;
    char *End = At + ReadFile.Size;
    
    hand_info Hand1Info, Hand2Info;
    
    while (At < End)
    {
        switch(*At)
        {
            case 'A': {Cards[LaidDownCount].Value = 14;} break;
            case 'K': {Cards[LaidDownCount].Value = 13;} break;
            case 'Q': {Cards[LaidDownCount].Value = 12;} break;
            case 'J': {Cards[LaidDownCount].Value = 11;} break;
            case 'T': {Cards[LaidDownCount].Value = 10;} break;
            case 'D': {Cards[LaidDownCount].Suit = 0; ++LaidDownCount;} break;
            case 'H': {Cards[LaidDownCount].Suit = 1; ++LaidDownCount;} break;
            case 'C': {Cards[LaidDownCount].Suit = 2; ++LaidDownCount;} break;
            case 'S': {Cards[LaidDownCount].Suit = 3; ++LaidDownCount;} break;
            default:
            {
                if ('2' <= *At && *At <= '9')
                {
                    Cards[LaidDownCount].Value = *At - '0';
                }
            }
        }
        
        if (LaidDownCount == 10)
        {
            LaidDownCount = 0;
            SortAndGetHandInfo(Cards, &Hand1Info);
            SortAndGetHandInfo(Cards + 5, &Hand2Info);
            b32 Tied = false;
            
#define TIE_BREAK(A, B)  \
if ((A) > (B)) goto Label_Win054; \
else if ((A) < (B)) goto Label_Lose054;
            TIE_BREAK(Hand1Info.Rank, Hand2Info.Rank);
            TIE_BREAK(Hand1Info.QuadValue, Hand2Info.QuadValue);
            TIE_BREAK(Hand1Info.TripleValue, Hand2Info.TripleValue);
            TIE_BREAK(Hand1Info.HighPairValue, Hand2Info.HighPairValue);
            TIE_BREAK(Hand1Info.LowPairValue, Hand2Info.LowPairValue);
            TIE_BREAK(Cards[0].Value, Cards[5+0].Value);
            TIE_BREAK(Cards[1].Value, Cards[5+1].Value);
            TIE_BREAK(Cards[2].Value, Cards[5+2].Value);
            TIE_BREAK(Cards[3].Value, Cards[5+3].Value);
            TIE_BREAK(Cards[4].Value, Cards[5+4].Value);
#undef TIE_BREAK
            
            Tied = true;
            Label_Win054:  if (!Tied) ++Result;
            Label_Lose054: ;
        }
        ++At;
    }
    
    EndTemporaryMemory(TempMemory);
    printf("%u\n", Result);
}

#define ARRAY_COUNT_055 128

internal void
SetNumber055(u8 *Number, u32 StartingNumber)
{
    u32 Index = 0;
    while (StartingNumber > 0)
    {
        Number[Index] = StartingNumber % 10;
        StartingNumber /= 10;
        ++Index;
    }
    while (Index < ARRAY_COUNT_055)
    {
        Number[Index] = 0;
        ++Index;
    }
}

internal void
ReverseNumber055(u8 *Source, u8 *Dest)
{
    u32 HighIndex = ARRAY_COUNT_055 - 1;
    while (Source[HighIndex] == 0)
    {
        Assert(HighIndex > 0);
        --HighIndex;
    }
    u32 Index = 0;
    for(; Index <= HighIndex; ++Index)
    {
        Dest[Index] = Source[HighIndex - Index];
    }
    for(; Index < ARRAY_COUNT_055; ++Index)
    {
        Dest[Index] = 0;
    }
    
}

internal void
SumNumber055(u8 *Source, u8 *Dest)
{
    u8 Carry = 0;
    for (u32 Index = 0; Index < ARRAY_COUNT_055; ++Index)
    {
        u8 Sum = Dest[Index] + Source[Index] + Carry;
        Dest[Index] = Sum % 10;
        Carry = Sum / 10;
    }
}

internal b32
IsPalindrome055(u8 *Number)
{
    u32 HighIndex = ARRAY_COUNT_055 - 1;
    while (Number[HighIndex] == 0)
    {
        Assert(HighIndex > 0);
        --HighIndex;
    }
    b32 Result = true;
    for (u32 Index = 0; Index < HighIndex; ++Index)
    {
        if (Number[Index] != Number[HighIndex - Index])
        {
            Result = false;
            break;
        }
    }
    return Result;
}

internal void
PrintNumber055(u8 *Number)
{
    s32 HighIndex = ARRAY_COUNT_055 - 1;
    while (Number[HighIndex] == 0)
    {
        Assert(HighIndex > 0);
        --HighIndex;
    }
    while (HighIndex >= 0)
    {
        printf("%u ", Number[HighIndex]);
        --HighIndex;
    }
}

internal void
Problem055()
{
    // Consider the process adding the reverse of a number (with reversed digits) to itself.
    // If you repeat this and never end up with a palindrome number, then it's a Lychrel number.
    // We don't actually know if Lychrel numbers exist (maybe 196?).
    // Assume that a number is "Lychrel" when doing this process 49 times never gives a palindrome.
    // How many "Lychrel" numbers are there below 10,000 ?
    
    u32 Result = 0;
    u8 Number[ARRAY_COUNT_055];
    u8 Copy[ARRAY_COUNT_055];
    for (u32 StartingNumber = 1; StartingNumber < 10000; ++StartingNumber)
    {
        SetNumber055(Number, StartingNumber);
        //printf("Number : ");
        //PrintNumber055(Number);
        b32 IsLychrel = true;
        for (u32 IterationCount = 0; IterationCount < 49; ++IterationCount)
        {
            ReverseNumber055(Number, Copy);
            SumNumber055(Copy, Number);
            //printf(" transform %u: ", IterationCount);
            //PrintNumber055(Number);
            if (IsPalindrome055(Number))
            {
                //printf(" is palindrome");
                IsLychrel = false;
                break;
            }
        }
        if (IsLychrel)
            ++Result;
        //printf("\n");
    }
    
    printf("%u\n", Result);
}

#define ARRAY_COUNT_056 200

internal void
SetNumber056(u32 *Dest, u32 Number)
{
    for (u32 Index = 0; Index < ARRAY_COUNT_056; ++Index)
    {
        Dest[Index] = Number % 10;
        Number /= 10;
    }
}

internal void
MultNumber056(u32 *Dest, u32 Factor)
{
    u32 Carry = 0;
    for (u32 Index = 0; Index < ARRAY_COUNT_056; ++Index)
    {
        Dest[Index] = Dest[Index]*Factor + Carry;
        Carry = Dest[Index] / 10;
        Dest[Index] %= 10;
    }
}

internal void
Problem056()
{
    // Considering natural numbers a^b where a,b < 100, what is the maximum digital sum?
    u32 Number[ARRAY_COUNT_056];
    u32 Result = 0;
    for (u32 Base = 1; Base < 100; ++Base)
    {
        SetNumber056(Number, Base);
        for (u32 Exp = 2; Exp < 100; ++Exp)
        {
            MultNumber056(Number, Base);
            u32 SumDigit = 0;
            for (u32 Index = 0; Index < ARRAY_COUNT_056; ++Index)
                SumDigit += Number[Index];
            if (SumDigit > Result)
                Result = SumDigit;
        }
    }
    
    printf("%u\n", Result);
}

#define ARRAY_COUNT_057 400
internal u32
Sum057(u32 *Source, u32 *Dest)
{
    u32 DigitCount = 0;
    u32 Carry = 0;
    for (u32 Index = 0; Index < ARRAY_COUNT_057; ++Index)
    {
        Dest[Index] += Source[Index] + Carry;
        Carry = Dest[Index] / 10;
        Dest[Index] %= 10;
        if (Dest[Index])
            DigitCount = Index + 1;
    }
    return DigitCount;
}

internal u32
SumToOther057(u32 *SourceA, u32 *SourceB, u32 *Dest)
{
    u32 DigitCount = 0;
    u32 Carry = 0;
    for (u32 Index = 0; Index < ARRAY_COUNT_057; ++Index)
    {
        Dest[Index] = SourceA[Index] + SourceB[Index] + Carry;
        Carry = Dest[Index] / 10;
        Dest[Index] %= 10;
        if (Dest[Index])
            DigitCount = Index + 1;
    }
    return DigitCount;
}



internal void
PrintNumber057(u32 *Number)
{
    s32 HighIndex = ARRAY_COUNT_057 - 1;
    while (Number[HighIndex] == 0)
    {
        Assert(HighIndex > 0);
        --HighIndex;
    }
    while (HighIndex >= 0)
    {
        printf("%u ", Number[HighIndex]);
        --HighIndex;
    }
}

internal void
Problem057()
{
    // In the first 1000 expansions of sqrt(2) expressed as a continued fraction,
    // how many fractions contain a numerator with more digits than the denominator?
    
    // first two expansions: 
    // 1 + 1/2 = 3/2
    // 1 + 1/(2 + 1/2) = 7/5
    // expansions follow this recursion: u(n+1) = 1 + 1 / (1+u(n))
    // if u(n) = a / b, then u(n+1) = (a + 2b) / (a + b).
    // We have gcd(a+2b, a+b) = gcd(b, a+b) = gcd(b, a), 
    // so if a/b is in reduced form, then (a+2b) / (a+b) also is.
    
    u32 A[ARRAY_COUNT_057] = {0};
    u32 B[ARRAY_COUNT_057] = {0};
    A[0] = 1;
    B[0] = 1;
    
    u32 CopyB[ARRAY_COUNT_057];
    u32 Result = 0;
    for (u32 Expansion = 1; Expansion <= 1000; ++Expansion)
    {
        for (u32 Index = 0; Index < ArrayCount(B); ++Index)
            CopyB[Index] = B[Index];
        u32 DigitCountB = Sum057(A, B);                // overwrite B   b = a+b;
        u32 DigitCountA = SumToOther057(B, CopyB, A);  // overwrite A   a = b + oldb
        
#if 0
        printf("Expansion %u -------------------\n", Expansion);
        PrintNumber057(A);
        printf("\n");
        PrintNumber057(B);
        printf("\n");
        printf("DigitCountA: %u, DigitCountB: %u \n", DigitCountA, DigitCountB);
#endif
        if (DigitCountA > DigitCountB)
            ++Result;
    }
    printf("%u\n", Result);
}

internal void
Problem058()
{
    // Consider the diagonal elements of a spiral of numbers like in problem 28.
    // As you add a layer to the spiral, the side length increases by two (starts at 1, always odd).
    // What is the side length of the square spiral for which the ratio of primes along the diagonals
    // first falls below 10% ?
    
    u32 LastTerm = 9;
    u32 TermCount = 5;
    u32 PrimesCount = 3;
    u32 Result = 0;
    for (u32 LayerIndex = 2; ; ++LayerIndex)
    {
        u32 TermIncrement = LayerIndex*2;
        u32 Term1 = LastTerm + TermIncrement;
        u32 Term2 = Term1 + TermIncrement;
        u32 Term3 = Term2 + TermIncrement;
        u32 Term4 = Term3 + TermIncrement;
        LastTerm = Term4;
        PrimesCount += IsPrime(Term1) + IsPrime(Term2) + IsPrime(Term3) + IsPrime(Term4);
        TermCount += 4;
        if (PrimesCount * 10 < TermCount)
        {
            Result = 2*LayerIndex + 1;
            break;
        }
    }
    printf("%u\n", Result);
}

internal void
Problem059(memory_arena *Arena)
{
    // The associated file has been XOR-ciphered with a 3-letter ascii key (lower case characters)
    // that you cycle through.
    // Find the sum of the ascii values in the original text.
    
    // NOTE(vincent): source file is not in binary format :(
    // gotta have to parse it.
    
    char *Key = "exp"; // NOTE(vincent): find this by trial and error
    u32 Result = 0;
    
    temporary_memory TempMemory = BeginTemporaryMemory(Arena);
    push_read_file ReadFile = PushReadFile(Arena, "text059");
    Assert(ReadFile.Success && ReadFile.Memory && ReadFile.Size > 0);
    char *At = (char *)ReadFile.Memory;
    char *End = At + ReadFile.Size;
    u32 KeyOffset = 0;
    char SourceAsciiNumber = 0;
    b32 InNumber = false;
    while (At < End)
    {
        if ('0' <= *At && *At <= '9')
        {
            SourceAsciiNumber = 10*SourceAsciiNumber + (*At - '0');
            InNumber = true;
        }
        else if (InNumber)
        {
            char Original = SourceAsciiNumber ^ Key[KeyOffset];
            //printf("%c", Original);
            Result += Original;
            KeyOffset = (KeyOffset + 1) % 3;
            SourceAsciiNumber = 0;
            InNumber = false;
        }
        ++At;
    }
    EndTemporaryMemory(TempMemory);
    printf("%u\n", Result);
}

internal b32
CanConcatenate060(u32 A, u32 B)
{
    b32 Result = false;
    u32 PowerTenA = 1;
    u32 PowerTenB = 1;
    for (u32 CopyA = A; CopyA > 0; CopyA /= 10)
        PowerTenA *= 10;
    for (u32 CopyB = B; CopyB > 0; CopyB /= 10)
        PowerTenB *= 10;
    
    u32 First = PowerTenA*B + A;
    u32 Second = PowerTenB*A + B;
    if (IsPrime(First) && IsPrime(Second))
    {
        Result = true;
    }
    return Result;
}

internal void
Problem060(memory_arena *Arena)
{
    // Find the lowest sum for a set of five primes for which any two primes 
    // concatenate (in both orders) to produce another prime.
    u32 Result = 0xffffffff;
    
    // TODO(vincent): This is super slow :(
    
    temporary_memory PrimesMemory = BeginTemporaryMemory(Arena);
    u32 *Primes = PushArray(Arena, 5, u32);
    Primes[0] = 3;
    Primes[1] = 7;
    Primes[2] = 11;
    Primes[3] = 13;
    Primes[4] = 17;
    u32 HighestValueCheckedForPrimeGen = 17;
    u32 MaxPrimeValueToGenerate = 26000;
    // NOTE(vincent): Once you see an actual sum value coming out at around 26000,
    // then you know that checking all primes under that value proves that you 
    // eventually find the minimal sum.
    
    Assert(CanConcatenate060(7, 109));
    
    //u32 LoopCount1 = 0;
    //u32 LoopCount2 = 0;
    //u32 LoopCount3 = 0;
    //u32 LoopCount4 = 0;
    
    s32 PrimesCount = 5;
    for (u32 Candidate = HighestValueCheckedForPrimeGen + 2; 
         Candidate <= MaxPrimeValueToGenerate; 
         Candidate+=2)
    {
        if (IsPrime(Candidate))
        {
            ++PrimesCount;
            *PushStruct(Arena, u32) = Candidate;
        }
        HighestValueCheckedForPrimeGen = Candidate;
    }
    
    for (s32 i = 0; i < PrimesCount - 4; ++i)
    {
        //++LoopCount1;
        u32 Prime1 = Primes[i];
        //printf("%u\n", Prime1);
        u32 Sum = Prime1;
        if (Sum + Primes[i+1] + Primes[i+2] + Primes[i+3] + Primes[i+4] >= Result)
            break;
        temporary_memory Primes2Mem = BeginTemporaryMemory(Arena);
        s32 Primes2Count = 0;
        u32 *Primes2 = (u32 *)(Arena->Base + Arena->Used);
        for (s32 Index = i + 1; Index < PrimesCount; ++Index)
        {
            u32 Prime2 = Primes[Index];
            if (CanConcatenate060(Prime1, Prime2))
            {
                *PushStruct(Arena, u32) = Prime2;
                ++Primes2Count;
            }
        }
        
        
        for (s32 j = 0; j < Primes2Count - 3; ++j)
        {
            //++LoopCount2;
            u32 Prime2 = Primes2[j];
            Sum += Prime2;
            if (Sum + Primes2[j+1] + Primes2[j+2] + Primes2[j+3] >= Result)
            {
                Sum -= Prime2;
                break;
            }
            temporary_memory Primes3Mem = BeginTemporaryMemory(Arena);
            s32 Primes3Count = 0;
            u32 *Primes3 = (u32 *)(Arena->Base + Arena->Used);
            
            for (s32 Index = j + 1; Index < Primes2Count; ++Index)
            {
                u32 Prime3 = Primes2[Index];
                if (CanConcatenate060(Prime1, Prime3) && CanConcatenate060(Prime2, Prime3))
                {
                    *PushStruct(Arena, u32) = Prime3;
                    ++Primes3Count;
                }
            }
            
            for (s32 k = 0; k < Primes3Count - 2; ++k)
            {
                //++LoopCount3;
                u32 Prime3 = Primes3[k];
                Sum += Prime3; //Prime1 + Prime2 + Prime3;
                if (Sum + Primes3[k+1] + Primes3[k+2] >= Result)
                {
                    Sum -= Prime3;
                    break;
                }
                temporary_memory Primes4Mem = BeginTemporaryMemory(Arena);
                s32 Primes4Count = 0;
                u32 *Primes4 = (u32 *)(Arena->Base + Arena->Used);
                
                for (s32 Index = k + 1; Index < Primes3Count; ++Index)
                {
                    u32 Prime4 = Primes3[Index];
                    Assert(Prime4 > 0);
                    if (CanConcatenate060(Prime1, Prime4) && CanConcatenate060(Prime2, Prime4) && CanConcatenate060(Prime3, Prime4))
                    {
                        *PushStruct(Arena, u32) = Prime4;
                        ++Primes4Count;
                    }
                }
                
                for (s32 l = 0; l < Primes4Count - 1; ++l)
                {
                    //++LoopCount4;
                    u32 Prime4 = Primes4[l];
                    Sum += Prime4;
                    if (Sum + Primes4[l+1] >= Result)
                    {
                        Sum -= Prime4;
                        break;
                    }
                    for (s32 Index = l + 1; Index < Primes4Count; ++Index)
                    {
                        u32 Prime5 = Primes4[Index];
                        Sum += Prime5;
                        if (Sum >= Result)
                            break;
                        if (CanConcatenate060(Prime1, Prime5) && CanConcatenate060(Prime2, Prime5) && 
                            CanConcatenate060(Prime3, Prime5) && CanConcatenate060(Prime4, Prime5))
                        {
                            //printf("%u %u %u %u %u -> %u\n", 
                            //Prime1, Prime2, Prime3, Prime4, Prime5, Sum);
                            Result = Sum;
                            break;
                        }
                        Sum -= Prime5;
                    }
                    Sum -= Prime4;
                }
                EndTemporaryMemory(Primes4Mem);
                Sum -= Prime3;
            }
            EndTemporaryMemory(Primes3Mem);
            Sum -= Prime2;
        }
        EndTemporaryMemory(Primes2Mem);
    }
    EndTemporaryMemory(PrimesMemory);
    
    //printf("loop counts: %u %u %u %u\n", LoopCount1, LoopCount2, LoopCount3, LoopCount4);
    printf("%u\n", Result);
}

typedef struct
{
    u32 Count;
    u32 *Base;
}  integer_list;

internal void
PushNumbers061(memory_arena *Arena, u32 Case, integer_list *List)
{
    u32 n = 0;
    for (;;)
    {
        u32 Figurate;
        switch (Case)
        {
            case 0: Figurate = (n*(n+1))/2; break;
            case 1: Figurate = n*n; break;
            case 2: Figurate = (n*(3*n-1))/2; break;
            case 3: Figurate = (n*(2*n-1)); break;
            case 4: Figurate = (n*(5*n-3))/2; break;
            case 5: Figurate = (n*(3*n-2)); break;
            InvalidDefaultCase;
        }
        
        if (1000 <= Figurate && Figurate < 10000 && (Figurate / 10) % 10 != 0)
        {
            u32 *Number = PushStruct(Arena, u32);
            if (List->Count == 0)
                List->Base = Number;
            List->Base[List->Count] = *Number;
            ++List->Count;
        }
        
        if (Figurate >= 10000)
            break;
    }
}

typedef struct
{
    union
    {
        struct
        {
            integer_list Triangles;
            integer_list Squares;
            integer_list Pentagons;
            integer_list Hexagons;
            integer_list Heptagons;
            integer_list Octagons;
        };
        integer_list Figurates[6];
    };
} figurates_061;

internal void
Problem061(memory_arena *Arena)
{
    // triangle, square, pentagonal, hexagonal, heptagonal, octagonal are all figurate (polygonal)
    // numbers
    // n(n+1)/2    triangle
    // n^2         square
    // n(3n-1)/2   pentagonal
    // n(2n-1)     hexagonal
    // n(5n-3)/2   heptagonal
    // n(3n-2)     octagonal
    
    // The ordered set (8128, 2882, 8281) has three interesting properties:
    // - cyclic (last two digits of each number are the first two digits of the next, 
    //   including last number with first number)
    // - each polygonal type represented by a different number in the set (triangle, square, pentagonal)
    // - only set of 4-digit numbers with this property.
    
    // Find the sum of the only ordered set of six cyclic 4-digit numbers for which each polygonal type
    // is represented by a different number in the set.
    // a b c d    a > 0  c > 0
    
    temporary_memory TempMem = BeginTemporaryMemory(Arena);
    figurates_061 Fig = {};
    
    for (u32 i = 0; i < ArrayCount(Fig.Figurates); ++i)
    {
        PushNumbers061(Arena, i, Fig.Figurates + i);
    }
    
    // 1 - choose figurate type order (5! possibilities)
    // 2 - loop through triangulars.
    //     search space for a given triangular should be very small
    permutation_choices PermChoices;
    u32 ChosenNumbers[6];
    u32 Perm[5];
    u32 Sum;
    for (u32 TriangleIndex = 0; TriangleIndex < Fig.Triangles.Count; ++TriangleIndex)
    {
        u32 Triangle = Fig.Triangles.Base[TriangleIndex];
        InitPermutation(5, &PermChoices);
        do
        {
            WritePermutationArrayFromChoices(&PermChoices, Perm, 1);
            integer_list Lists[5];
            for (u32 i = 0; i < 5; ++i)
                Lists[i] = Fig.Figurates[Perm[i]];
            
            for (u32 i = 0; i < Lists[0].Count; ++i)
            {
                ChosenNumbers[1] = Lists[0].Base[i];
                if (ChosenNumbers[0] % 100 < ChosenNumbers[1] / 100) break;
                if (ChosenNumbers[0] % 100 > ChosenNumbers[1] / 100) continue;
                
                for (u32 j = 0; j < Lists[1].Count; ++j)
                {
                    ChosenNumbers[2] = Lists[1].Base[j];
                    if (ChosenNumbers[1] % 100 < ChosenNumbers[2] / 100) break;
                    if (ChosenNumbers[1] % 100 > ChosenNumbers[2] / 100) continue;
                    
                    for (u32 k = 0; k < Lists[2].Count; ++k)
                    {
                        ChosenNumbers[3] = Lists[2].Base[k];
                        if (ChosenNumbers[2] % 100 < ChosenNumbers[3] / 100) break;
                        if (ChosenNumbers[2] % 100 > ChosenNumbers[3] / 100) continue;
                        
                        for (u32 l = 0; l < Lists[3].Count; ++l)
                        {
                            ChosenNumbers[4] = Lists[3].Base[l];
                            if (ChosenNumbers[3] % 100 < ChosenNumbers[4] / 100) break;
                            if (ChosenNumbers[3] % 100 > ChosenNumbers[4] / 100) continue;
                            
                            for (u32 m = 0; m < Lists[4].Count; ++m)
                            {
                                ChosenNumbers[5] = Lists[4].Base[m];
                                if (ChosenNumbers[4] % 100 < ChosenNumbers[5] / 100) break;
                                if (ChosenNumbers[4] % 100 > ChosenNumbers[5] / 100) continue;
                                
                                if (ChosenNumbers[5] % 100 == ChosenNumbers[0] / 100)
                                {
                                    Sum = 0;
                                    for (u32 TermIndex = 0; TermIndex < ArrayCount(ChosenNumbers);
                                         ++TermIndex)
                                        Sum += TermIndex;
                                }
                                
                            }
                        }
                    }
                }
            }
        } while (AdvancePermutation(&PermChoices));
    }
    
    EndTemporaryMemory(TempMem);
    printf("%u\n", Sum);
}

internal u32
Hash32(u32 a)
{
    // Taken from here: https://gist.github.com/badboy/6267743
    a = (a+0x7ed55d16) + (a << 12);
    a = (a^0xc761c23c) ^ (a >> 19);
    a = (a+0x165667b1) + (a << 5);
    a = (a+0xd3a2646c) ^ (a << 9);
    a = (a+0xfd7046c5) + (a << 3);
    a = (a^0xb55a4f09) ^ (a >> 16);
    return a;
}

typedef struct set_062 set_062;
struct set_062
{
    u32 Key;
    u64 MinValue;
    u32 Count;
    set_062 *Next;
};

internal set_062*
GetSet062(memory_arena *Arena, set_062 *Nodes, u32 HashInput, u32 HashBufferCount)
{
    u32 Hash = Hash32(HashInput);
    
    set_062 *Node = Nodes + (Hash & (HashBufferCount - 1));
    for (;;)
    {
        if (Node->Key)
        {
            if (Node->Key == HashInput)
            {
                break;
            }
            if (!Node->Next)
            {
                Node->Next = PushZeroStruct(Arena, set_062);
                Node = Node->Next;
                Node->Key = HashInput;
                break;
            }
            else
                Node = Node->Next;
        }
        else
        {
            Node->Key = HashInput;
            break;
        }
    }
    
    return Node;
}

internal u32
GetHashKey062(u64 Cube)
{
    u32 Digits[10] = {0};
    u64 CopyCube = Cube;
    while (CopyCube > 0)
    {
        Digits[CopyCube%10]++;
        CopyCube /= 10;
    }
    
    u32 Key = 1;
    for (int i = 0; i < ArrayCount(Digits); ++i)
    {
        u32 D = Digits[i];
        Key = (Key << (D+1)) | 1;
    }
    
    return Key;
}

internal void
Problem062(memory_arena *Arena)
{
    // 41063625 is the smallest cube which has exactly three permutations of its digits which are also cube.
    // Find the smallest cube for which exactly five permutations of its digits are also cube.
    temporary_memory TempMem = BeginTemporaryMemory(Arena);
    
    
    u32 HashBufferCount = 4096;
    
    set_062 *Candidates[1000];
    u32 CandidatesCount = 0;
    
    set_062 *Nodes = PushZeroArray(Arena, HashBufferCount, set_062);
    
    u64 Answer = -1;
    Assert(Answer > (1lu << 63));
    
#if 1
    // NOTE(vincent): This approach uses Hash-table lookups, takes about 5ms
    for (u64 i = 1; i < 10000; ++i)
    {
        // loop over cubes, build sets based on digits that are accessible in constant time.
        u64 Cube = i*i*i;
        
        u32 Key = GetHashKey062(Cube);
        set_062 *Set = GetSet062(Arena, Nodes, Key, HashBufferCount);
        if (Set->Count == 0)
            Set->MinValue = Cube;
        ++Set->Count;
        if (Set->Count == 5)
        {
            Assert(CandidatesCount < ArrayCount(Candidates));
            Candidates[CandidatesCount++] = Set;
        }
        
    }
    
    for (u32 i = 0; i < CandidatesCount; ++i)
    {
        set_062 *Set = Candidates[i];
        if (Set->Count == 5 && Answer > Set->MinValue)
        {
            Answer = Set->MinValue;
            break;
        }
    }
#else
    
    // NOTE(vincent): Naive quadratic approach, takes like 3.5s
    for (u64 i = 1; i < 10000; ++i)
    {
        u64 CubeA = i*i*i;
        
        u32 SiblingsCount = 1;
        
        u8 DigitsA[10] = {0};
        u64 CopyA = CubeA;
        while (CopyA > 0)
        {
            DigitsA[CopyA%10]++;
            CopyA /= 10;
        }
        for (u64 j = i + 1; j < 10000; ++j)
        {
            u64 CubeB = j*j*j;
            
            u8 DigitsB[10] = {0};
            u64 CopyB = CubeB;
            while (CopyB > 0)
            {
                DigitsB[CopyB%10]++;
                CopyB /= 10;
            }
            
            b32 SameDigits = true;
            for (u32 i = 0; i < ArrayCount(DigitsA); ++i)
            {
                if (DigitsA[i] != DigitsB[i])
                {
                    SameDigits = false;
                }
            }
            if (SameDigits)
            {
                SiblingsCount++;
            }
        }
        if (SiblingsCount == 5)
        {
            Answer = CubeA;
            break;
        }
    }
#endif
    EndTemporaryMemory(TempMem);
    printf("%lu\n", Answer);
}

internal u32
DigitCount(u64 N)
{
    u32 Count = 0;
    if (N == 0) 
        Count = 1;
    while (N > 0)
    {
        N /= 10;
        ++Count;
    }
    return Count;
}

internal void
Problem063(void)
{
    // How many n-digit positive integers exist which are also an nth power?
    u32 Base = 2;
    u32 Power = 1;
    
    // 9^21 is the only one that doesn't fit in u64, and that's easy to verify, so I'm counting it here.
    u32 Answer = 1;  
    
    for (u64 Base = 1; Base < 10; ++Base)
    {
        u64 Number = Base;
        u32 Exponent = 1;
        for (;;)
        {
            if (DigitCount(Number) == Exponent)
                ++Answer;
            else if (DigitCount(Number) < Exponent)
                break;
            Number *= Base;
            ++Exponent;
        }
    }
    printf("%u\n", Answer);
}

internal u32
Square(u32 N)
{
    u32 Result = N*N;
    Assert(N < 60000);
    return Result;
}

internal void
Problem064(void)
{
    u32 LastSquareRadicand = 1;
    u32 RootLastSquareRadicand = 1;
    
    u32 Answer = 0;
    
    for (u32 Radicand = 2; Radicand < 10000; ++Radicand)
    {
        if (Square(RootLastSquareRadicand+1) == Radicand)
        {
            ++RootLastSquareRadicand;
            continue;
        }
        
        Assert(Square(RootLastSquareRadicand) < Radicand && Radicand < Square(RootLastSquareRadicand+1));
        
        u32 A = RootLastSquareRadicand;
        u32 B = 1;
        u32 k = RootLastSquareRadicand;
        
        u32 PeriodLength = 1;
        for (;; ++PeriodLength)
        {
            u32 AA = Square(A);
            Assert((Radicand - AA) % B == 0); // NOTE(vincent): You can prove this by induction.
            
            u32 NextB = (Radicand - AA) / B;
            u32 NextA = RootLastSquareRadicand;
            
            // now need to decrease NextA by the least amount >= 0 such that NextA is congruent to -A mod NextB.
            u32 r = (NextA + A) % NextB;
            NextA -= r;
            
            k = (NextA + A) / NextB;
            A = NextA;
            B = NextB;
            
            if (A == RootLastSquareRadicand && B == 1)
                break;
        }
        Assert(PeriodLength < 1000);
        Answer += (PeriodLength & 1);
    }
    printf("%u\n", Answer);
}

internal u32
Coef065(u32 N)
{
    u32 Coef = 1;
    if (N == 1)
        Coef = 2;
    else if (N % 3 == 0)
        Coef = 2*(N/3);
    return Coef;
}

typedef struct
{
    u64 High;
    u64 Low;
} u128;


internal u128
Sum128(u128 A, u128 B)
{
    u128 Result = {0};
    u64 MaxLowTerm = (A.Low < B.Low ? B.Low : A.Low);
    Result.Low = A.Low + B.Low;
    Result.High = A.High + B.High + (Result.Low < MaxLowTerm);
    return Result;
}

internal u128
Mult128(u128 A, u128 B)
{
    u128 Result = {0};
    
    while (B.Low || B.High)
    {
        if (B.Low & 1) 
            Result = Sum128(Result, A);
        
        // A <<= 1
        A.High <<= 1;
        if ((A.Low & (1 << 63)))
            A.High |= 1;
        A.Low <<= 1;
        
        // B >>= 1
        B.Low >>= 1;
        B.Low |= ((B.High & 1) << 63);
        B.High >>= 1;
    }
    return Result;
}

internal u128
U128ToString(u128 A)
{
    // TODO(vincent): write u128 as a string:
    // probably need to compute quotient and remainder by 10 repeatedly, convert to ascii, reverse chars.
    
}

internal void
Problem065(void)
{
    // TODO(vincent): This crashes when ConvergentNumber is set to 42,
    // probably due to overflow.
    u32 ConvergentNumber = 41;
    
    u32 N = ConvergentNumber;
    u64 Numerator = Coef065(N);
    u64 Denominator = 1;
    --N;
    for (; N > 0; --N)
    {
        u32 Coef = Coef065(N);
        u64 Temp = Numerator;
        Numerator = Denominator;
        Denominator = Temp;
        
        // add Coef to fraction
        Numerator += Coef * Denominator;
        u64 g = GCD64(Numerator, Denominator);
        Numerator /= g;
        Denominator /= g;
    }
    
    printf("Num: %20lu\nDen: %20lu\n", Numerator, Denominator);
}

internal void
Problem066(void)
{
    // Consider diophantine equations of the form:
    // x^2 - Dy^2 = 1
    // when D = 13, the minimal solution in x is (x,y) = (649, 180).
    // It can be assumed that there are no solutions in positive integers when D is square.
    // Minimal solutions in x:
    // (3,2) when D = 2
    // (2,1) when D = 3
    // (9,4) when D = 5
    // (5,2) when D = 6
    // (8,3) when D = 7
    // By considering minimal solutions in x for D <= 7, the largest x is obtained when D = 5.
    // Find the value of D <= 1000 in minimal solutions of x for which the largest value of x is obtained.
    
    // TODO(vincent): look up Pell's diophantine equation
    
    u32 LargestX = 1;
    u32 Answer = 2;
    
    u32 NextSquare = 4;
    u32 NextSquareIndex = 2;
    
    for (u32 D = 2; D <= 1000; ++D)
    {
        printf("%u\n", D);
        if (D == NextSquare)
        {
            ++NextSquareIndex;
            NextSquare = NextSquareIndex * NextSquareIndex;
            continue;
        }
        for (u64 x = 1; ; ++x)
        {
            //printf("%u\n", x);
            u64 SquareXMinusOne = x*x - 1;
            for (u32 y = 1; ; ++y)
            {
                // TODO(vincent): find y faster
                u64 SquareY = y*y;
                u64 DTerm = D*SquareY;
                if (DTerm == SquareXMinusOne)
                {
                    if (x > LargestX)
                    {
                        LargestX = x;
                        Answer = D;
                    }
                    goto Goto_NextD;
                }
                if (DTerm > SquareXMinusOne)
                    break;
            }
        }
        
        Goto_NextD: ;
    }
    
    printf("%u\n", Answer);
    
}

#if WINDOWS_BUILD
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/mman.h>
#include <time.h>
#endif
int main(int argc, char **argv)
{
    
#if WINDOWS_BUILD
    LARGE_INTEGER Frequency;
    LARGE_INTEGER CounterStart;
    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&CounterStart);
    void *Memory = VirtualAlloc(0, Gigabytes(1), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
    struct timespec StartWallClock = {};
    clock_gettime(CLOCK_MONOTONIC, &StartWallClock);
    void *Memory = mmap(0, Gigabytes(1), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    Assert(Memory);
#endif
    memory_arena Arena;
    InitializeArena(&Arena, Gigabytes(1), Memory);
    
#if 0
    Problem001();
    Problem002();
    Problem003();
    Problem004();
    Problem005();
    Problem006();
    Problem007();
    Problem008();
    Problem009();
    Problem010();
    printf("\n");
    
    Problem011(&Arena);
    Problem012();
    Problem013(&Arena);
    Problem014();
    Problem015();
    Problem016();
    Problem017();
    Problem018();
    Problem019();
    Problem020();
    printf("\n");
    
    Problem021();
    Problem022(&Arena);
    Problem023(&Arena);
    Problem024();
    Problem025();
    Problem026(&Arena);
    Problem027();
    Problem028();
    Problem029();
    Problem030();
    printf("\n");
    
    Problem031();
    Problem032(&Arena);
    Problem033();
    Problem034();
    Problem035();
    Problem036();
    Problem037();
    Problem038();
    Problem039();
    Problem040();
    printf("\n");
    
    Problem041();
    Problem042(&Arena);
    Problem043();
    Problem044(&Arena);
    Problem045();
    Problem046(&Arena);
    Problem047(&Arena);
    Problem048();
    Problem049();
    Problem050(&Arena);
    printf("\n");
    
    Problem051();
    Problem052();
    Problem053(&Arena);
    Problem054(&Arena);
    Problem055();
    Problem056();
    Problem057();
    Problem058();
    Problem059(&Arena);
    Problem060(&Arena);
    printf("\n");
#endif
    
    //Problem061(&Arena); // TODO(vincent): 
    Problem062(&Arena);
    Problem063();
    Problem064();
    Problem065(); // TODO(vincent): 
    //Problem066(); // TODO(vincent): 
#if 0
#endif
    
    //DrawModTableQuadratic(3, -1, 0);
    
#if WINDOWS_BUILD
    LARGE_INTEGER CounterEnd;
    QueryPerformanceCounter(&CounterEnd);
    f64 ElapsedSeconds = ((f64)CounterEnd.QuadPart - (f64)CounterStart.QuadPart) / (f64)Frequency.QuadPart;
    printf("Elapsed seconds: %lf\n", ElapsedSeconds);
#else
    // tv_sec  tv_nsec
    struct timespec EndWallClock = {};
    clock_gettime(CLOCK_MONOTONIC, &EndWallClock);
    struct timespec DiffTime;
    if (StartWallClock.tv_nsec <= EndWallClock.tv_nsec)
    {
        DiffTime.tv_sec = EndWallClock.tv_sec - StartWallClock.tv_sec;
        DiffTime.tv_nsec = EndWallClock.tv_nsec - StartWallClock.tv_nsec;
    }
    else
    {
        DiffTime.tv_sec = EndWallClock.tv_sec - StartWallClock.tv_sec - 1;
        DiffTime.tv_nsec = 1000*1000*1000 + EndWallClock.tv_nsec - StartWallClock.tv_nsec;
    }
    
    f64 ElapsedSeconds = DiffTime.tv_sec + ((f64)DiffTime.tv_nsec / (1000.0 * 1000.0 * 1000.0));
    printf("Elapsed seconds: %lf\n", ElapsedSeconds);
#endif
    CheckArena(&Arena);
    return 0;
}
