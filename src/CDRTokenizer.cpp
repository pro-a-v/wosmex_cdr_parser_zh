#include "CDRTokenizer.h"




template <typename Container>
Container& split( Container&  result, const typename Container::value_type& s,  const typename Container::value_type& delimiters  )
{
  result.clear();
  size_t current;
  size_t next = -1;

  do
  {
    current = next + 1;
    next = s.find_first_of( delimiters, current );
    result.push_back( s.substr( current, next - current ) );
  }
  while (next != Container::value_type::npos);

  return result;
}

CDRTokenizer::CDRTokenizer()
{
    fields.clear();
}

CDRTokenizer::~CDRTokenizer()
{
    fields.clear();
}

bool CDRTokenizer::Split(char *s)
{
    split( fields, s, "|" );
    //
    if (fields.size()< 20 )
    {
        sprintf(ErrorText,"Fields count < 20 File format chandged.");
        return false;

    }
    //print( fields );

return true;
}



string CDRTokenizer::GetField(FIELD_NAME a)
{
    return fields[a];
}


void CDRTokenizer::AddFileCDR(_cdr new_cdr)
{
    new_cdr.MESSAGE_ID[9]=0x00;
    cdrs.push_back(new_cdr);
}


CDRTokenizer::_cdr CDRTokenizer::GetCDRbyNumber(long number)
{
    if (number<0) return cdrs[0];

    if (number>cdrs.size()) cdrs[cdrs.size()];

 return cdrs[number];
}

long CDRTokenizer::GetCDRCount()
{
    return cdrs.size();
}

