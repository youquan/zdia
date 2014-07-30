#ifndef _CONF_H_
#define _CONF_H_

typedef char * diam_id_t;
typedef char * diam_realm_t;

typedef struct {
    const char *            conf_file;

    diam_id_t               identity;
    diam_realm_t            realm;

    /* transport */
    list_t *                servers;

    /* peer */

    /* realm */
} conf_t;

conf_t *conf_new();
void    conf_free(conf_t *conf);
void    conf_init(conf_t *conf);

int     conf_parse(conf_t *conf, const char *file);

#endif
