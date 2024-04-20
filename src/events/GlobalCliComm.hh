#ifndef GLOBALCLICOMM_HH
#define GLOBALCLICOMM_HH

#include "CliComm.hh"
#include "hash_map.hh"
#include "xxhash.hh"
#include <array>
#include <memory>
#include <mutex>
#include <vector>

namespace openmsx {

class CliListener;

class GlobalCliComm final : public CliComm
{
public:
	GlobalCliComm() = default;
	GlobalCliComm(const GlobalCliComm&) = delete;
	GlobalCliComm(GlobalCliComm&&) = delete;
	GlobalCliComm& operator=(const GlobalCliComm&) = delete;
	GlobalCliComm& operator=(GlobalCliComm&&) = delete;
	~GlobalCliComm();

	CliListener* addListener(std::unique_ptr<CliListener> listener);
	std::unique_ptr<CliListener> removeListener(CliListener& listener);

	// Before this method has been called commands send over external
	// connections are not yet processed (but they keep pending).
	void setAllowExternalCommands();

	// CliComm
	void log(LogLevel level, std::string_view message, float fraction) override;
	void update(UpdateType type, std::string_view name,
	            std::string_view value) override;
	void updateFiltered(UpdateType type, std::string_view name,
	            std::string_view value) override;

private:
	void updateHelper(UpdateType type, std::string_view machine,
	                  std::string_view name, std::string_view value);

private:
	std::array<hash_map<std::string, std::string, XXHasher>, NUM_UPDATES> prevValues;

	std::vector<std::unique_ptr<CliListener>> listeners; // unordered
	std::mutex mutex; // lock access to listeners member
	bool delivering = false;
	bool allowExternalCommands = false;

	friend class MSXCliComm;
};

} // namespace openmsx

#endif
