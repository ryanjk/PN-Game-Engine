#ifndef PN_COMMAND_H
#define PN_COMMAND_H

#include <vector>
#include <memory>

namespace pn {
	class Command {
	public:
		virtual ~Command() {}
		virtual void execute() = 0;
	};
}

using CommandQueue = std::vector < std::shared_ptr<pn::Command> > ;

#endif