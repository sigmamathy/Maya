#pragma once

#include "./core.hpp"

struct MayaSaveSlot
{
	using Variant = std::variant<int, float, std::string>;
	using Object = std::unordered_map<std::string, Variant>;
	std::unordered_map<std::string, Object> objects;
};

class MayaSaveIO
{
public:
	MayaSaveIO(std::string const& filepath);
	void Import(MayaSaveSlot& slot);
	void Export(MayaSaveSlot const& slot);

private:
	std::string filepath;
};