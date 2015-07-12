#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define USE_TERMINAL

#ifdef USE_TERMINAL
#include "../kernel/include/terminal.h"
#endif

int putchar(int ic)
{
	// todo implement a write system call

	#ifdef USE_TERMINAL
	kernel::terminal_putchar((char)ic);
	#endif

	return ic;
}


static void print(const char* data, size_t data_length)
{
	for ( size_t i = 0; i < data_length; i++ )
		putchar((int) ((const unsigned char*) data)[i]);
}


void print_unsigned_decimal(unsigned int i)
{
	unsigned upper = i / 10;
	unsigned lower = i % 10;
	if(upper > 0)
	{
		print_unsigned_decimal(upper);
	}
	char digit = (char)lower + '0';
	print(&digit, sizeof(digit));
}

char get_hex_symbol(unsigned value)
{
	return "0123456789abcdef"[value & 0xf];
}

void print_hexadecimal(unsigned int i)
{
	unsigned upper_digits = i >> 4;
	unsigned lower_digit = i & 0xf;
	if(upper_digits > 0)
	{
		print_hexadecimal(upper_digits);
	}
	char symbol = get_hex_symbol(lower_digit);
	print(&symbol, sizeof(symbol));
}

int printf(const char* __restrict format, ...)
{
	va_list parameters;
	va_start(parameters, format);

	int written = 0;
	size_t amount;
	bool rejected_bad_specifier = false;

	while ( *format != '\0' )
	{
		if ( *format != '%' )
		{
		print_c:
			amount = 1;
			while ( format[amount] && format[amount] != '%' )
				amount++;
			print(format, amount);
			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format;

		if ( *(++format) == '%' )
			goto print_c;

		if ( rejected_bad_specifier )
		{
		incomprehensible_conversion:
			rejected_bad_specifier = true;
			format = format_begun_at;
			goto print_c;
		}

		if ( *format == 'c' )
		{
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			print(&c, sizeof(c));
		}
		else if ( *format == 's' )
		{
			format++;
			const char* s = va_arg(parameters, const char*);
			print(s, strlen(s));
		}
		else if ( *format == 'u' )
		{
			format++;
			unsigned int u = va_arg(parameters, unsigned int);
			print_unsigned_decimal(u);
		}
		else if ( *format == 'x' )
		{

			format++;
			unsigned int u = va_arg(parameters, unsigned int);
			print_hexadecimal(u);
		}
		else
		{
			goto incomprehensible_conversion;
		}
	}

	va_end(parameters);

	return written;
}

int puts(const char* string)
{
	return printf("%s\n", string);
}

