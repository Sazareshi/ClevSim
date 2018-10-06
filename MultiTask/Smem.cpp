#include "stdafx.h"
#include "Smem.h"

smemdef_tag::~smemdef_tag() {
	delete &stSmem;
}

CSmem::CSmem()
{
}

CSmem::~CSmem()
{
}
