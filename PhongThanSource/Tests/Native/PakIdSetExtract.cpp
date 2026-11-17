#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ucl/ucl.h>

#pragma pack(push, 1)
struct PakHeader
{
	char signature[4];
	unsigned long count;
	unsigned long indexOffset;
	unsigned long dataOffset;
	unsigned long crc32;
	unsigned char reserved[12];
};

struct PakIndex
{
	unsigned long id;
	unsigned long offset;
	long size;
	long compressedSizeFlag;
};
#pragma pack(pop)

static const unsigned long TYPE_FILTER = 0xff000000UL;
static const unsigned long TYPE_UCL = 0x01000000UL;
static const unsigned long TYPE_UPL_NEW = 0x20000000UL;

static const PakIndex* Find(const PakIndex* rows, unsigned long count, unsigned long id)
{
	long low = 0, high = (long)count - 1;
	while (low <= high)
	{
		long mid = low + (high - low) / 2;
		if (id < rows[mid].id) high = mid - 1;
		else if (id > rows[mid].id) low = mid + 1;
		else return &rows[mid];
	}
	return NULL;
}

int main(int argc, char** argv)
{
	if (argc != 5)
	{
		fprintf(stderr, "usage: PakIdSetExtract <pak> <id-list> <output-dir> <catalog>\n");
		return 2;
	}
	FILE* pak = fopen(argv[1], "rb");
	FILE* ids = fopen(argv[2], "rb");
	FILE* catalog = fopen(argv[4], "wb");
	if (!pak || !ids || !catalog) return 3;
	PakHeader header;
	if (fread(&header, 1, sizeof(header), pak) != sizeof(header) ||
		memcmp(header.signature, "PACK", 4) != 0 || !header.count) return 4;
	PakIndex* index = (PakIndex*)malloc(header.count * sizeof(PakIndex));
	if (!index || fseek(pak, header.indexOffset, SEEK_SET) ||
		fread(index, sizeof(PakIndex), header.count, pak) != header.count) return 5;
	CreateDirectoryA(argv[3], NULL);
	fprintf(catalog, "TrapIdHex,Status,OriginalBytes,StoredBytes,Compression\r\n");
	unsigned long requested = 0, resolved = 0, missing = 0;
	char line[64];
	while (fgets(line, sizeof(line), ids))
	{
		char* end = NULL;
		unsigned long id = strtoul(line, &end, 16);
		if (!id) continue;
		++requested;
		const PakIndex* entry = Find(index, header.count, id);
		if (!entry)
		{
			fprintf(catalog, "0x%08lX,MISSING,0,0,NONE\r\n", id);
			++missing;
			continue;
		}
		unsigned long stored = ((unsigned long)entry->compressedSizeFlag) & ~TYPE_FILTER;
		unsigned long method = ((unsigned long)entry->compressedSizeFlag) & TYPE_FILTER;
		unsigned char* source = (unsigned char*)malloc(stored);
		unsigned char* output = (unsigned char*)malloc(entry->size);
		if (!source || !output || fseek(pak, entry->offset, SEEK_SET) ||
			fread(source, 1, stored, pak) != stored) return 6;
		bool ok = false;
		if (method == 0 && stored == (unsigned long)entry->size)
		{
			memcpy(output, source, stored);
			ok = true;
		}
		else if (method == TYPE_UCL || method == TYPE_UPL_NEW)
		{
			ucl_uint outputSize = (ucl_uint)entry->size;
			ok = ucl_nrv2b_decompress_safe_8(source, stored, output, &outputSize, NULL) == UCL_E_OK &&
				outputSize == (ucl_uint)entry->size;
		}
		free(source);
		if (!ok) return 7;
		char outputPath[MAX_PATH];
		sprintf(outputPath, "%s\\%08lX.bin", argv[3], id);
		FILE* destination = fopen(outputPath, "wb");
		if (!destination || fwrite(output, 1, entry->size, destination) != (unsigned long)entry->size) return 8;
		fclose(destination);
		free(output);
		fprintf(catalog, "0x%08lX,RESOLVED,%ld,%lu,0x%08lX\r\n",
			id, entry->size, stored, method);
		++resolved;
	}
	free(index);
	fclose(pak);
	fclose(ids);
	fclose(catalog);
	printf("REQUESTED=%lu\nRESOLVED=%lu\nMISSING=%lu\n", requested, resolved, missing);
	return 0;
}
