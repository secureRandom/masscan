#ifndef OUTPUT_H
#define OUTPUT_H
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "main-src.h"
#include "unusedparm.h"
#include "masscan-app.h"

struct Masscan;
struct Output;
enum ApplicationProtocol;

struct OutputType {
    const char *file_extension;
    void *(*create)(struct Output *out);
    void (*open)(struct Output *out, FILE *fp);
    void (*close)(struct Output *out, FILE *fp);
    void (*status)(struct Output *out, FILE *fp, time_t timestamp, int status, unsigned ip, unsigned port, unsigned reason, unsigned ttl);
    void (*banner)(struct Output *out, FILE *fp, time_t timestamp, unsigned ip, unsigned ip_proto, unsigned port, enum ApplicationProtocol proto, const unsigned char *px, unsigned length);
};

struct Output
{
    const struct Masscan *masscan;
    struct Source src[8];
    FILE *fp;
    const struct OutputType *funcs;
    time_t next_rotate;
    time_t last_rotate;
    unsigned is_banner:1;
    unsigned period;
    unsigned offset;
    struct {
        struct {
            uint64_t open;
            uint64_t closed;
            uint64_t banner;
        } tcp;
        struct {
            uint64_t open;
            uint64_t closed;
        } udp;
        struct {
            uint64_t echo;
            uint64_t timestamp;
        } icmp;
		struct {
			uint64_t open;
		} arp;
    } counts;

    struct {
        unsigned ip;
        unsigned port;
        ptrdiff_t fd;
        uint64_t outstanding;
        unsigned state;
    } redis;
};

const char *proto_from_status(unsigned status);
const char *normalize_string(const unsigned char *px, size_t length, char *buf, size_t buf_len);


extern const struct OutputType text_output;
extern const struct OutputType xml_output;
extern const struct OutputType binary_output;
extern const struct OutputType null_output;
extern const struct OutputType redis_output;


struct Output *output_create(const struct Masscan *masscan);
void output_destroy(struct Output *output);

void output_report_status(struct Output *output, time_t timestamp, 
    int status, unsigned ip, unsigned port, unsigned reason, unsigned ttl);


typedef void (*OUTPUT_REPORT_BANNER)(
                struct Output *output, time_t timestamp,
                unsigned ip, unsigned ip_proto, unsigned port, 
                unsigned proto,
                const unsigned char *px, unsigned length);

void output_report_banner(
                struct Output *output, 
                time_t timestamp,
                unsigned ip, unsigned ip_proto, unsigned port, 
                unsigned proto,
                const unsigned char *px, unsigned length);



const char *status_string(int x);
const char *reason_string(int x, char *buffer, size_t sizeof_buffer);


#endif
