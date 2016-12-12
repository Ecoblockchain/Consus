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

// STL
#include <sstream>

// consus
#include "common/txman_configuration.h"

e::unpacker
consus :: txman_configuration(e::unpacker up,
                              cluster_id* cid,
                              version_id* vid,
                              uint64_t* flags,
                              std::vector<data_center>* dcs,
                              std::vector<txman_state>* txmans,
                              std::vector<paxos_group>* txman_groups,
                              std::vector<kvs>* kvss)
{
    return up >> *cid >> *vid >> *flags >> *dcs >> *txmans >> *txman_groups >> *kvss;
}

std::string
consus :: txman_configuration(const cluster_id& cid,
                              const version_id& vid,
                              uint64_t,
                              const std::vector<data_center>& dcs,
                              const std::vector<txman_state>& txmans,
                              const std::vector<paxos_group>& txman_groups,
                              const std::vector<kvs>& kvss)
{
    std::ostringstream ostr;
    ostr << cid << "\n"
         << vid << "\n";

    if (dcs.empty())
    {
        ostr << "default data center only\n";
    }
    else if (dcs.size() == 1)
    {
        ostr << "1 configured data center:\n";
    }
    else
    {
        ostr << dcs.size() << " configured data centers:\n";
    }

    for (size_t i = 0; i < dcs.size(); ++i)
    {
        ostr << dcs[i] << "\n";
    }

    if (txmans.empty())
    {
        ostr << "no transaction managers\n";
    }
    else if (txmans.size() == 1)
    {
        ostr << "1 transaction manager:\n";
    }
    else
    {
        ostr << txmans.size() << " transaction managers:\n";
    }

    for (size_t i = 0; i < txmans.size(); ++i)
    {
        ostr << txmans[i] << "\n";
    }

    if (txman_groups.empty())
    {
        ostr << "no paxos groups\n";
    }
    else if (txman_groups.size() == 1)
    {
        ostr << "1 paxos group:\n";
    }
    else
    {
        ostr << txman_groups.size() << " paxos groups:\n";
    }

    for (size_t i = 0; i < txman_groups.size(); ++i)
    {
        ostr << txman_groups[i] << "\n";
    }

    if (kvss.empty())
    {
        ostr << "no key value stores\n";
    }
    else if (kvss.size() == 1)
    {
        ostr << "1 key value store\n";
    }
    else
    {
        ostr << kvss.size() << " key value stores:\n";
    }

    for (size_t i = 0; i < kvss.size(); ++i)
    {
        ostr << kvss[i] << "\n";
    }

    return ostr.str();
}
