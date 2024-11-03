#include "base.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static Allocator string_allocator = {
	.alloc = nullptr,
	.realloc = nullptr,
	.free = nullptr,
};

static void check_string_allocator() {
	if (string_allocator.alloc == nullptr || string_allocator.realloc == nullptr || string_allocator.free == nullptr) {
		fprintf(stderr, "string_allocator not setted");
		abort();
	}
}

void set_string_allocator(Allocator allocator) {
	string_allocator = allocator;
	check_string_allocator();
}

String8 create_string(u32 reserve) {
	check_string_allocator();

	String8 string = {
		.data = (c8*) string_allocator.alloc(reserve),
		.len = 0,
		.reserved = reserve,
	};
	
	memset((void*) string.data, 0, string.len);

	return string;
}

String8 create_string_from(const c8* in_string) {
	u32 len = strlen(in_string);
	String8 string = create_string(len);
	strcpy(string.data, in_string);
	string.len = len;

	return string;
}

void assign_string(String8* string, const c8* in_string) {
	check_string_allocator();

	u32 len = strlen(in_string);
	if (len > string->reserved) {
		string_allocator.free((void*) string->data);
		string->data = (c8*) string_allocator.alloc(len);
		string->reserved = len;
	}

	memset((void*) string->data, 0, string->len);
	strcpy(string->data, in_string);
}

void destroy_string(String8* string) {
	check_string_allocator();

	if (string->data != nullptr) {
		string_allocator.free((void*) string->data);
		string->data = nullptr;
		string->len = 0;
	}
}

String8 String8::operator=(const c8* in_string) {
	if (reserved != 0) {
		assign_string(this, in_string);
	} else {
		// FIXME: make initialization work
		*this = create_string_from(in_string);
	}

	return *this;
}