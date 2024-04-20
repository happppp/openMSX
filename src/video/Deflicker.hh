#ifndef DEFLICKER_HH
#define DEFLICKER_HH

#include "FrameSource.hh"

#include <memory>
#include <span>

namespace openmsx {

class RawFrame;

class Deflicker final : public FrameSource
{
public:
	explicit Deflicker(std::span<std::unique_ptr<RawFrame>, 4> lastFrames);
	Deflicker(const Deflicker&) = default;
	Deflicker(Deflicker&&) = default;
	Deflicker& operator=(const Deflicker&) = default;
	Deflicker& operator=(Deflicker&&) = default;
	~Deflicker();

	void init();

	[[nodiscard]] unsigned getLineWidth(unsigned line) const override;
	[[nodiscard]] const void* getLineInfo(
		unsigned line, unsigned& width,
		void* buf, unsigned bufWidth) const override;

private:
	std::span<std::unique_ptr<RawFrame>, 4> lastFrames;
};

} // namespace openmsx

#endif
