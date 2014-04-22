//==================================================================================================
//
// File:    Hash.cpp
// Author:  Jason Jackson
// Date:    September 27, 2008
//
// Hashing functions that convert arrays of data into a single integer
//==================================================================================================

#include "EngineDeps.h"
#include "Hash.h"

// When defined, additional checks will be put in place that will confirm that when a string matches, the hash matches
#define DBG_HASH_CHECK_COLLISIONS 


static uint32 _sHash32( const uint8 *  pData, uint32 sizeInBytes );
static uint32 _sHash32( const uint16 * pData, uint32 sizeInBytes );
static uint32 _sHash32( const uint32 * pData, uint32 sizeInBytes );

static const uint32 _sHashTab[256] = {
    0x46d1e192, 0x66edf9aa, 0x927fc9e5, 0xa53baacc, 0x29b47658, 0x5a411a01, 0x0e66d5bd, 0x0dd5b1db,
    0xcb38340e, 0x04d4ebb6, 0x98bc4f54, 0x36f20f2c, 0x4a3047ed, 0x1ec1e0eb, 0x568c0c1f, 0x6a731432,
    0x81367fc6, 0xe3e25237, 0xe7f64884, 0x0fa59f64, 0x4f3109de, 0xf02d61f5, 0x5daec03b, 0x7f740e83, 
    0x056ff2d8, 0x2026cc0a, 0x7ac2112d, 0x82c55605, 0xb0911ef2, 0xa7b88e4c, 0x89dca282, 0x4b254d27, 
    0x7694a6d3, 0xd229eadd, 0x8e8f3738, 0x5bee7a55, 0x012eb6ab, 0x08dd28c8, 0xb5abc274, 0xbc7931f0, 
    0xf2396ed5, 0xe4e43d97, 0x943f4b7f, 0x85d0293d, 0xaed83a88, 0xc8f932fc, 0xc5496f20, 0xe9228173,
    0x9b465b7d, 0xfda26680, 0x1ddeab35, 0x0c4f25cb, 0x86e32faf, 0xe59fa13a, 0xe192e2c4, 0xf147da1a, 
    0x67620a8d, 0x5c9a24c5, 0xfe6afde2, 0xacad0250, 0xd359730b, 0xf35203b3, 0x96a4b44d, 0xfbcacea6, 
    0x41a165ec, 0xd71e53ac, 0x835f39bf, 0x6b6bde7e, 0xd07085ba, 0x79064e07, 0xee5b20c3, 0x3b90bd65,
    0x5827aef4, 0x4d12d31c, 0x9143496e, 0x6c485976, 0xd9552733, 0x220f6895, 0xe69def19, 0xeb89cd70, 
    0xc9bb9644, 0x93ec7e0d, 0x2ace3842, 0x2b6158da, 0x039e9178, 0xbb5367d7, 0x55682285, 0x4315d891, 
    0x19fd8906, 0x7d8d4448, 0xb4168a03, 0x40b56a53, 0xaa3e69e0, 0xa25182fe, 0xad34d16c, 0x720c4171,
    0x9dc3b961, 0x321db563, 0x8b801b9e, 0xf5971893, 0x14cc1251, 0x8f4ae962, 0xf65aff1e, 0x13bd9dee, 
    0x5e7c78c7, 0xddb61731, 0x73832c15, 0xefebdd5b, 0x1f959aca, 0xe801fb22, 0xa89826ce, 0x30b7165d, 
    0x458a4077, 0x24fec52a, 0x849b065f, 0x3c6930cd, 0xa199a81d, 0xdb768f30, 0x2e45c64a, 0xff2f0d94,
    0x4ea97917, 0x6f572acf, 0x653a195c, 0x17a88c5a, 0x27e11fb5, 0x3f09c4c1, 0x2f87e71b, 0xea1493e4, 
    0xd4b3a55e, 0xbe6090be, 0xaf6cd9d9, 0xda58ca00, 0x612b7034, 0x31711dad, 0x6d7db041, 0x8ca786b7, 
    0x09e8bf7a, 0xc3c4d7ea, 0xa3cd77a8, 0x7700f608, 0xdf3de559, 0x71c9353f, 0x9fd236fb, 0x1675d43e, 
    0x390d9e9a, 0x21ba4c6b, 0xbd1371e8, 0x90338440, 0xd5f163d2, 0xb140fef9, 0x52f50b57, 0x3710cf67, 
    0x4c11a79c, 0xc6d6624e, 0x3dc7afa9, 0x34a69969, 0x70544a26, 0xf7d9ec98, 0x7c027496, 0x1bfb3ba3, 
    0xb3b1dc8f, 0x9a241039, 0xf993f5a4, 0x15786b99, 0x26e704f7, 0x51503c04, 0x028bb3b8, 0xede5600c,
    0x9cb22b29, 0xb6ff339b, 0x7e771c43, 0xc71c05f1, 0x604ca924, 0x695eed60, 0x688ed0bc, 0x3e0b232f, 
    0xf8a39c11, 0xbae6e67c, 0xb8cf75e1, 0x970321a7, 0x5328922b, 0xdef3df2e, 0x8d0443b0, 0x2885e3ae, 
    0x6435eed1, 0xcc375e81, 0xa98495f6, 0xe0bff114, 0xb2da3e4f, 0xc01b5adf, 0x507e0721, 0x6267a36a, 
    0x181a6df8, 0x7baff0c0, 0xfa6d6c13, 0x427250b2, 0xe2f742d6, 0xcd5cc723, 0x2d218be7, 0xb91fbbb1, 
    0x9eb946d0, 0x1c180810, 0xfc81d602, 0x0b9c3f52, 0xc2ea456f, 0x1165b2c9, 0xabf4ad75, 0x0a56fc8c, 
    0x12e0f818, 0xcadbcba1, 0x2586be56, 0x952c9b46, 0x07c6a43c, 0x78967df3, 0x477b2e49, 0x2c5d7b6d, 
    0x8a637272, 0x59acbcb4, 0x74a0e447, 0xc1f8800f, 0x35c015dc, 0x230794c2, 0x4405f328, 0xec2adba5, 
    0xd832b845, 0x6e4ed287, 0x48e9f7a2, 0xa44be89f, 0x38cbb725, 0xbf6ef4e6, 0xdc0e83fa, 0x54238d12, 
    0xf4f0c1e3, 0xa60857fd, 0xc43c64b9, 0x00c851ef, 0x33d75f36, 0x5fd39866, 0xd1efa08a, 0xa0640089, 
    0x877a978b, 0x99175d86, 0x57dfacbb, 0xceb02de9, 0xcf4d5c09, 0x3a8813d4, 0xb7448816, 0x63fa5568, 
    0x06be014b, 0xd642fa7b, 0x10aa7c90, 0x8082c88e, 0x1afcba79, 0x7519549d, 0x490a87ff, 0x8820c3a0,
};





//==================================================================================================
// 
//    Hash32::Hash32
//    
// Description: 
//    Constructs a hash from an array of bytes
//    
// Parameters: 
//    [const u8* pData] - Start of the data
//    [uint32 size] - number of u8s in the array to hash
//    
//==================================================================================================
Hash32::Hash32( const uint8 * pData, uint32 size )
{
    mValue = _sHash32( pData, size );
}


//==================================================================================================
// 
//    Hash32::Hash32
//    
// Description: 
//    Constructor that computes a hash from a data array
//    
// Parameters: 
//    [const u8* pStart] - Start of the data to hash
//    [const u8* pEnd] - One past the end of the data to hash
//    
//==================================================================================================
Hash32::Hash32( const uint8* pStart, const uint8* pEnd )
{
    mValue = _sHash32( pStart, pEnd - pStart - 1);
}


//==================================================================================================
// 
//    StringHash32::StringHash32
//    
// Description: 
//    Constructor
//    
// Parameters: 
//    [const std::string& str] - string that will be hashed
//    
//==================================================================================================
StringHash32::StringHash32( const std::string& str )
{
    Compute( str.c_str() );
#ifdef DEBUG
    mStr = str;
#endif
}


//==================================================================================================
// 
//    StringHash32::StringHash32
//    
// Description: 
//    Constructor
//    
// Parameters: 
//    [const char* pStr] - null-terminated string that will be hashed 
//    
//==================================================================================================
StringHash32::StringHash32( const char* pStr )
{
    Compute( pStr );
#ifdef DEBUG
    mStr = pStr;
#endif
}


//==================================================================================================
// 
//    StringHash32::Compute
//    
// Description: 
//    Computes a hash from a null-terminated string. Does not use the data hashing functions in 
//    order to get rid of the extra strlen() call.
//    
// Parameters: 
//    [const char* pStr] - null-terminated string
//    
// Return: 
//    [void] - none
//==================================================================================================
void StringHash32::Compute( const char* pStr )
{
    // We want null strings to map to zero
    mHash = 0;

    if( !pStr )
        return;

    for( uint32 c = *pStr; c; pStr++ )
        mHash = (mHash >> 8) ^ _sHashTab[(mHash & 0xff) ^ c];

}


#if defined(DEBUG) && defined(DBG_HASH_CHECK_COLLISIONS)

StringHash32& StringHash32::operator=( const StringHash32& rhs )
{
    if( this != &rhs )
    {
        mHash   = rhs.mHash;
        mStr    = rhs.mStr;
    }
    return *this;
}

bool StringHash32::operator==( const StringHash32& rhs ) const 
{
    ASSERT( (mHash == rhs.mHash) == (mStr == rhs.mStr)) && "Hash Collision" );
    return mHash == rhs.mHash;
}

bool StringHash32::operator!=( const StringHash32& rhs ) const 
{
    ASSERT( (mHash == rhs.mHash) == (mStr == rhs.mStr)) && "Hash Collision" );
    return mHash != rhs.mHash;
}

bool StringHash32::operator<=( const StringHash32& rhs ) const 
{ 
    ASSERT( (mHash == rhs.mHash) == (mStr == rhs.mStr)) && "Hash Collision" );
    return mHash <= rhs.mHash; 
}

bool StringHash32::operator>=( const StringHash32& rhs ) const 
{
    ASSERT( (mHash == rhs.mHash) == (mStr == rhs.mStr)) && "Hash Collision" );
    return mHash >= rhs.mHash; 
}

bool StringHash32::operator< ( const StringHash32& rhs ) const 
{
    ASSERT( (mHash == rhs.mHash) == (mStr == rhs.mStr)) && "Hash Collision" );
    return mHash <  rhs.mHash; 
}

bool StringHash32::operator> ( const StringHash32& rhs ) const 
{
    ASSERT( (mHash == rhs.mHash) == (mStr == rhs.mStr)) && "Hash Collision" );
    return mHash >  rhs.mHash; 
}

StringHash32::operator bool() const 
{
    ASSERT( (mHash == 0) == (mStr.Empty()) && "Hash Collision" );
    return mHash > 0; 
}

#else
StringHash32& StringHash32::operator=( const StringHash32& rhs )    { if( this != &rhs ) { mHash = rhs.mHash; } return *this; }
bool StringHash32::operator==( const StringHash32& rhs ) const      { return mHash == rhs.mHash; }
bool StringHash32::operator!=( const StringHash32& rhs ) const      { return mHash != rhs.mHash; }
bool StringHash32::operator<=( const StringHash32& rhs ) const      { return mHash <= rhs.mHash; }
bool StringHash32::operator>=( const StringHash32& rhs ) const      { return mHash >= rhs.mHash; }
bool StringHash32::operator< ( const StringHash32& rhs ) const      { return mHash <  rhs.mHash; }
bool StringHash32::operator> ( const StringHash32& rhs ) const      { return mHash >  rhs.mHash; }
StringHash32::operator bool() const                                 { return mHash > 0; }

#endif



















#define mix(a,b,c)                  \
{                                   \
    a -= b; a -= c; a ^= (c >> 13); \
    b -= c; b -= a; b ^= (a <<  8); \
    c -= a; c -= b; c ^= (b >> 13); \
    a -= b; a -= c; a ^= (c >> 12); \
    b -= c; b -= a; b ^= (a << 16); \
    c -= a; c -= b; c ^= (b >>  5); \
    a -= b; a -= c; a ^= (c >>  3); \
    b -= c; b -= a; b ^= (a << 10); \
    c -= a; c -= b; c ^= (b >> 15); \
}

static uint32 _sHash32( const uint8* pData, uint32 length )
{
    // Set up the internal state
    register uint32 len    = length;
    register uint32 a      = 0x9e3779b9;
    register uint32 b      = 0x9e3779b9;   // the golden ratio; an arbitrary value
    register uint32 c      = 0x1c8cfbff;   // the previous hash value

    while( len >= 12 )
    {
        a += pData[0] + ((uint32)pData[1] << 8) + ((uint32)pData[2]  << 16) + ((uint32)pData[3]  << 24);
        b += pData[4] + ((uint32)pData[5] << 8) + ((uint32)pData[6]  << 16) + ((uint32)pData[7]  << 24);
        c += pData[8] + ((uint32)pData[9] << 8) + ((uint32)pData[10] << 16) + ((uint32)pData[11] << 24);

        mix(a, b, c);

        pData   += 12; 
        len     -= 12;
    }

    c += length;

    switch(len)
    {
        case 11: c += (uint32)pData[10] << 24;
        case 10: c += (uint32)pData[9]  << 16;
        case 9 : c += (uint32)pData[8]  << 8;
          // the first byte of c is reserved for the length
        case 8 : b += (uint32)pData[7]  << 24;
        case 7 : b += (uint32)pData[6]  << 16;
        case 6 : b += (uint32)pData[5]  << 8;
        case 5 : b += pData[4];
        case 4 : a += (uint32)pData[3]  << 24;
        case 3 : a += (uint32)pData[2]  << 16;
        case 2 : a += (uint32)pData[1]  << 8;
        case 1 : a += pData[0];
    }

    mix(a,b,c);

    return c;
}

static uint32 _sHash32( const uint16* pData, uint32 length )
{
    // Set up the internal state
    register uint32 len    = length;
    register uint32 a      = 0x9e3779b9;
    register uint32 b      = 0x9e3779b9;   // the golden ratio; an arbitrary value
    register uint32 c      = 0x1c8cfbff;   // the previous hash value

    while( len >= 6 )
    {
        a += pData[0] + ((uint32)pData[1] << 16);
        b += pData[2] + ((uint32)pData[3] << 16);
        c += pData[4] + ((uint32)pData[5] << 16);

        mix(a, b, c);

        pData   += 6; 
        len     -= 6;
    }

    c += length;

    switch(len)
    {
        case 5: c += (uint32)pData[4] << 16;
        case 4: c += (uint32)pData[3] << 16;
        case 3: c += (uint32)pData[2];
        case 2: c += (uint32)pData[1] << 16;
        case 1: c += (uint32)pData[0];
    }

    mix(a,b,c);

    return c;
}


//==================================================================================================
// 
//    _sHash32
//    
// Description: 
//    
//    
// Parameters: 
//    [const uint32* pData] - 
//    [uint32 length] - 
//    
// Return: 
//    [uint32] - Hashed value
//==================================================================================================
static uint32 _sHash32( const uint32* pData, uint32 length )
{
    // Set up the internal state
    register uint32 len    = length;
    register uint32 a      = 0x9e3779b9;
    register uint32 b      = 0x9e3779b9;   // the golden ratio; an arbitrary value
    register uint32 c      = 0x1c8cfbff;   // the previous hash value

    while( len >= 3 )
    {
        a += pData[0];
        b += pData[1];
        c += pData[2];

        mix(a, b, c);

        pData   += 3; 
        len     -= 3;
    }

    c += length;

    switch(len)
    {
        case 2: c += (uint32)pData[1];
        case 1: c += (uint32)pData[0];
    }

    mix(a,b,c);

    return c;
}