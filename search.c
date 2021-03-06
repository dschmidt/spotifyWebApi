/**  This file is part of SpotifyWebApi - <hugolm84@gmail.com> ===
 *
 *   Copyright 2011,Hugo Lindström <hugolm84@gmail.com>
 *
 *
 *   Inspiration and reused functions has come from Spotifys spshell example
 *   and that code is:
 *   Copyright (c) 2006-2010 Spotify Ltd
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 */
#include <string.h>
#include <jansson.h>
#include "spshell.h"
#include "cmd.h"

#ifdef USE_MYSQL
extern MYSQL* g_conn;
extern int _mysql_updateStats(MYSQL *, char*);
#endif

extern json_t *get_track(sp_track *track);

/**
 * @param  search   The search result
 */
static void get_search(sp_search *search)
{
        int i;
        json_t *json = json_object();

        json_t *tracks = json_array();
        json_object_set_new(json, "tracks", tracks);
        for (i = 0; i < sp_search_num_tracks(search); ++i){
            json_array_append_new(tracks, get_track(sp_search_track(search, i)));
        }
        json_object_set_new_nocheck(json, "query", json_string_nocheck(sp_search_query(search)));
        cmd_sendresponse(json, 200);
}

/**
 *
 */
static void search_complete(sp_search *search, void *userdata)
{


        if (sp_search_error(search) == SP_ERROR_OK){
             get_search(search);
        }
        else cmd_sendresponse(put_error(400, sp_error_message(sp_search_error(search))), 400);

        sp_search_release(search);

        cmd_done();
}



/**
 *
 */
static void search_usage(void)
{
        cmd_sendresponse(put_error(400, "Usage: search <query> Example: /search/artist:Madonna track:Like a prayer"), 400);
}


/**
 *
 */
int cmd_search(int argc, char **argv)
{
        char query[1024];
        int i;

        if (argc < 2) {
                search_usage();
                return -1;
        }

        query[0] = 0;
        for(i = 1; i < argc; i++)
                snprintf(query + strlen(query), sizeof(query) - strlen(query), "%s%s",
                         i == 1 ? "" : " ", argv[i]);
#ifdef USE_MYSQL
                _mysql_updateStats(g_conn, "search");
#endif
        sp_search_create(g_session, query, 0, 100, 0, 100, 0, 100, &search_complete, NULL);
        return 0;
}


