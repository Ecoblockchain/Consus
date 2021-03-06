// Copyright (c) 2015-2016, Robert Escriva, Cornell University
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright notice,
//       this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Consus nor the names of its contributors may be
//       used to endorse or promote products derived from this software without
//       specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef consus_kvs_daemon_h_
#define consus_kvs_daemon_h_

// STL
#include <string>

// LevelDB
#include <leveldb/db.h>
#include <leveldb/filter_policy.h>

// po6
#include <po6/net/location.h>
#include <po6/threads/thread.h>

// e
#include <e/compat.h>
#include <e/garbage_collector.h>
#include <e/state_hash_table.h>

// BusyBee
#include <busybee_mta.h>

// consus
#include "namespace.h"
#include "common/constants.h"
#include "common/coordinator_link.h"
#include "common/kvs.h"
#include "kvs/configuration.h"
#include "kvs/datalayer.h"
#include "kvs/lock_manager.h"
#include "kvs/lock_replicator.h"
#include "kvs/mapper.h"
#include "kvs/migrator.h"
#include "kvs/read_replicator.h"
#include "kvs/write_replicator.h"

BEGIN_CONSUS_NAMESPACE

class daemon
{
    public:
        daemon();
        ~daemon() throw ();

    public:
        int run(bool daemonize,
                std::string data,
                std::string log,
                std::string pidfile,
                bool has_pidfile,
                bool set_bind_to,
                po6::net::location bind_to,
                bool set_coordinator,
                const char* coordinator,
                const char* data_center,
                unsigned threads);

    private:
        struct coordinator_callback;
        class migration_bgthread;
        typedef e::state_hash_table<uint64_t, lock_replicator> lock_replicator_map_t;
        typedef e::state_hash_table<uint64_t, read_replicator> read_replicator_map_t;
        typedef e::state_hash_table<uint64_t, write_replicator> write_replicator_map_t;
        typedef e::state_hash_table<partition_id, migrator> migrator_map_t;
        friend class mapper;
        friend class lock_manager;
        friend class lock_replicator;
        friend class lock_state;
        friend class read_replicator;
        friend class write_replicator;
        friend class migrator;

    private:
        void loop(size_t thread);
        void process_read_lock(comm_id id, std::auto_ptr<e::buffer> msg, e::unpacker up);
        void process_read_unlock(comm_id id, std::auto_ptr<e::buffer> msg, e::unpacker up);
        void process_write_begin(comm_id id, std::auto_ptr<e::buffer> msg, e::unpacker up);
        void process_write_finish(comm_id id, std::auto_ptr<e::buffer> msg, e::unpacker up);
        void process_write_cancel(comm_id id, std::auto_ptr<e::buffer> msg, e::unpacker up);

        void process_rep_rd(comm_id id, std::auto_ptr<e::buffer> msg, e::unpacker up);
        void process_rep_wr(comm_id id, std::auto_ptr<e::buffer> msg, e::unpacker up);
        void process_raw_rd(comm_id id, std::auto_ptr<e::buffer> msg, e::unpacker up);
        void process_raw_rd_resp(comm_id id, std::auto_ptr<e::buffer> msg, e::unpacker up);
        void process_raw_wr(comm_id id, std::auto_ptr<e::buffer> msg, e::unpacker up);
        void process_raw_wr_resp(comm_id id, std::auto_ptr<e::buffer> msg, e::unpacker up);

        void process_lock_op(comm_id id, std::auto_ptr<e::buffer> msg, e::unpacker up);
        void process_raw_lk(comm_id id, std::auto_ptr<e::buffer> msg, e::unpacker up);
        void process_raw_lk_resp(comm_id id, std::auto_ptr<e::buffer> msg, e::unpacker up);
        void process_wound_xact(comm_id id, std::auto_ptr<e::buffer> msg, e::unpacker up);

        void process_migrate_syn(comm_id id, std::auto_ptr<e::buffer> msg, e::unpacker up);
        void process_migrate_ack(comm_id id, std::auto_ptr<e::buffer> msg, e::unpacker up);

    private:
        static std::string logid(const e::slice& table, const e::slice& key);

    private:
        configuration* get_config();
        void debug_dump();
        uint64_t generate_id();
        uint64_t resend_interval() { return PO6_SECONDS; }
        bool send(comm_id id, std::auto_ptr<e::buffer> msg);
        void pump();

    private:
        kvs m_us;
        e::garbage_collector m_gc;
        mapper m_busybee_mapper;
        std::auto_ptr<busybee_mta> m_busybee;
        std::auto_ptr<coordinator_callback> m_coord_cb;
        std::auto_ptr<coordinator_link> m_coord;
        configuration* m_config;
        std::vector<e::compat::shared_ptr<po6::threads::thread> > m_threads;
        std::auto_ptr<datalayer> m_data;
        lock_manager m_locks;
        lock_replicator_map_t m_repl_lk;
        read_replicator_map_t m_repl_rd;
        write_replicator_map_t m_repl_wr;
        migrator_map_t m_migrations;
        std::auto_ptr<migration_bgthread> m_migrate_thread;

    private:
        daemon(const daemon&);
        daemon& operator = (const daemon&);
};

END_CONSUS_NAMESPACE

#endif // consus_kvs_daemon_h_
