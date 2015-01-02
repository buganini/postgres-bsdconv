/*
 * Copyright (c) 2011-2014 Kuan-Chung Chiu <buganini@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "postgres.h"
#include <bsdconv.h>
#include "fmgr.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(Bsdconv);

Datum
Bsdconv(PG_FUNCTION_ARGS)
{
	text *_conversion=PG_GETARG_TEXT_P(0);
	text *str=PG_GETARG_TEXT_P(1);
	char *conversion=malloc(VARSIZE(_conversion) - VARHDRSZ + 1);
	memcpy(conversion, VARDATA(_conversion), VARSIZE(_conversion) - VARHDRSZ);
	conversion[VARSIZE(_conversion) - VARHDRSZ]=0;
	struct bsdconv_instance *ins=bsdconv_create(conversion);
	free(conversion);

	bsdconv_init(ins);
	ins->output_mode=BSDCONV_PREMALLOCED;
	ins->input.data=VARDATA(str);
	ins->input.len=VARSIZE(str) - VARHDRSZ;
	ins->input.flags=0;
	ins->input.next=NULL;
	ins->output.data=0;
	ins->flush=1;
	bsdconv(ins);

	text *new_text;
	int32 new_text_size;

	new_text_size=ins->output.len + VARHDRSZ;
	new_text=(text *) palloc(new_text_size);
	ins->output.data=VARDATA(new_text);
	SET_VARSIZE(new_text, new_text_size);

	bsdconv(ins);
	bsdconv_destroy(ins);

	PG_RETURN_TEXT_P(new_text);
}
