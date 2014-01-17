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
