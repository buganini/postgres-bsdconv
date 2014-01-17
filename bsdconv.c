/*
 * Copyright (c) 2011-2014 Kuan-Chung Chiu <buganini@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
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
