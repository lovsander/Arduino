#ifndef debug_debug_h
#define debug_debug_h
#include <Arduino.h>

 #define NDEBUG
 //#define DEBUG
// #define NLOG

// LOG*******************************************

#define LOG_LEVEL 2

#ifndef LOG_LEVEL
#define LOG_LEVEL 2
#endif

#ifndef NLOG

#define LOG_INIT( baud )                        \
  do                                           \
  {                                            \
    Serial.begin( baud );                     \
  }                                            \
  while( 0 )

#define LOGLN( msg )                            \
  do                                           \
  {                                            \
    Serial.println( F( msg ) );               \
  }                                            \
  while( 0 )

#define LOG( msg )                              \
  do                                           \
  {                                            \
    Serial.print( F( msg ) );                 \
  }                                            \
  while( 0 )

#define LOGVLN( var )                           \
  do                                           \
  {                                            \
    Serial.println( var );                    \
  }                                            \
  while( 0 )

#define LOGV( var )                             \
  do                                           \
  {                                            \
    Serial.print( var );                      \
  }                                            \
  while( 0 )

#else

#define LOG_INIT( baud )
#define LOGLN( msg )
#define LOG( msg )
#define LOGVLN( var )
#define LOGV( var )

#endif



#ifndef NDEBUG

// ASSERT********************************************
#define ASSERT( expr, msg )                     \
  if( !(expr) )                                \
  {                                            \
    Serial.print( "Assertion failed: " );     \
    Serial.print( #expr );                    \
    Serial.print( " -> " );                   \
    Serial.println( msg );                    \
    pinMode(LED_BUILTIN, OUTPUT); \
    while( 1 ) \
    { \
      digitalWrite(LED_BUILTIN, LOW); \
      delay(500); \
      digitalWrite(LED_BUILTIN, HIGH); \
      delay(500); \
    } \
  }


// INSIST********************************************
#define INSIST( stmnt, cmp, msg )               \
  ASSERT( (stmnt) cmp, msg )

#else // ifndef NDEBUG...

#define INSIST( stmnt, cmp, msg ) stmnt;
#define ASSERT( expr, msg )

#endif // ifndef NDEBUG...

#endif // ifndef debug_debug_h...
