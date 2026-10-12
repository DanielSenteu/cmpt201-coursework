#define _DEFAULT_SOURCE
#define _ISOC99_SOURCE

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define BLOCK_SIZE   128
#define HEAP_SIZE    256
#define BUF_SIZE     64


struct header
{
    uint64_t       size;
    struct header *next;
};


static void handle_error( const char *msg )
{
    perror( msg );
    exit( EXIT_FAILURE );
}



static void print_out( const char *fmt, void *data, size_t data_size )
{
    char    buf[ BUF_SIZE ];
    ssize_t len;

    if ( data_size == sizeof( uint64_t ) )
        len = snprintf( buf, BUF_SIZE, fmt, *( uint64_t * ) data );
    else
        len = snprintf( buf, BUF_SIZE, fmt, *( void ** ) data );

    if ( len < 0 )
    {
        handle_error( "snprintf" );
    }

    write( STDOUT_FILENO, buf, len );
}


static void print_block( char *start )
{
    size_t payload = BLOCK_SIZE - sizeof( struct header );
    char  *data    = start + sizeof( struct header );

    for ( size_t i = 0;  i < payload;  i++ )
    {
        uint64_t value = ( uint64_t ) data[ i ];
        print_out( "%lu\n", &value, sizeof( value ) );
    }
}


static void initialize_block( struct header *blk,
                              uint64_t       size,
                              struct header *next,
                              int            fill )
{
    blk->size = size;
    blk->next = next;

    memset( ( char * ) blk + sizeof( struct header ),
            fill,
            size - sizeof( struct header ) );
}



int main( void )
{
    char *heap_base = sbrk( 0 );

    if ( sbrk( HEAP_SIZE ) == ( void * ) -1 )
        handle_error( "sbrk" );


    struct header *block_a = ( struct header * ) heap_base;
    struct header *block_b = ( struct header * ) ( heap_base + BLOCK_SIZE );

    initialize_block( block_a, BLOCK_SIZE, NULL,    0 );
    initialize_block( block_b, BLOCK_SIZE, block_a, 1 );


    print_out( "first block: %p\n",  &block_a, sizeof( block_a ) );
    print_out( "second block: %p\n", &block_b, sizeof( block_b ) );

    print_out( "first block size: %lu\n",  &block_a->size, sizeof( block_a->size ) );
    print_out( "first block next: %p\n",   &block_a->next, sizeof( block_a->next ) );

    print_out( "second block size: %lu\n", &block_b->size, sizeof( block_b->size ) );
    print_out( "second block next: %p\n",  &block_b->next, sizeof( block_b->next ) );


    print_block( ( char * ) block_a );
    print_block( ( char * ) block_b );

    return 0;
}
