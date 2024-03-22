#pragma once

#include "json.h"
#include <optional>
#include <vector>
#include <string>

namespace json
{
	class DictItemContext;
	class KeyItemContext;
	class ValueItemContext;

	class ArrayItemContext;

	class Builder
	{
	public:
		Builder();
		DictItemContext StartDict();
		ArrayItemContext StartArray();
		KeyItemContext Key(std::string);
		Builder& Value(Node::Value);
		Builder& EndDict();
		Builder& EndArray();
		Node Build();


	private:
		std::optional<std::string> keys_;
		Node root_ = nullptr;
		std::vector<Node*> nodes_stack_;

		Node GetNode(json::Node::Value value);
	};

	class DictItemContext
	{
	public:
		DictItemContext(Builder& builder) : main_(builder) {}

		KeyItemContext Key(std::string);
		Builder& EndDict();

	private:
		Builder& main_;
	};

	class KeyItemContext
	{
	public:
		KeyItemContext(Builder& builder) : main_(builder) {}

		ValueItemContext Value(Node::Value);

		ArrayItemContext StartArray();
		DictItemContext StartDict();

	private:
		Builder& main_;
	};

	class ValueItemContext
	{
	public:
		ValueItemContext(Builder& builder) : main_(builder) {}

		KeyItemContext Key(std::string);
		Builder& EndDict();

	private:
		Builder& main_;
	};

	class ArrayItemContext
	{
	public:
		ArrayItemContext(Builder& builder) : main_(builder) {}

		ArrayItemContext StartArray();
		DictItemContext StartDict();
		ArrayItemContext Value(Node::Value);
		Builder& EndArray();

	private:
		Builder& main_;
	};
} //namespace json 