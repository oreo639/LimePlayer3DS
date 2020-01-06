#ifndef __LIME_NETFMT_H__
#define __LIME_NETFMT_H__

#include "player.hpp"
#include "net.hpp"

namespace Netfmt {
	std::unique_ptr<Decoder> GetFormat(const std::string& url, FileTransport* ftrans);
}

#endif
