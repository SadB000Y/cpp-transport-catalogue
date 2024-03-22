#include "json_builder.h"

json::Builder::Builder() {
	nodes_stack_.push_back(&root_);
}

json::DictItemContext json::Builder::StartDict()
{
	auto top_node = nodes_stack_.back();

	if (top_node->IsDict() && keys_.has_value())
	{
		auto& dict = std::get<Dict>(top_node->GetValue());
		auto it = dict.emplace(keys_.value(), Dict());
		nodes_stack_.push_back(&it.first->second);
		keys_.reset();
	}
	else if (top_node->IsArray())
	{
		auto& arr = std::get<Array>(top_node->GetValue());
		arr.emplace_back(Dict());
		*(nodes_stack_.back()) = arr;
		nodes_stack_.push_back(&arr.back());
	}
	else if (top_node->IsNull())
	{
		top_node->GetValue() = Dict();
	}
	else
		throw std::logic_error("Wrong context of StartDict()");

	return *this;
}

json::Builder& json::Builder::EndDict()
{
	auto top_node = nodes_stack_.back();

	if (top_node->IsDict() && !keys_.has_value())
		nodes_stack_.pop_back();
	else
		throw std::logic_error("Dictionary must exist");

	return *this;
}

json::ArrayItemContext json::Builder::StartArray()
{
	auto top_node = nodes_stack_.back();

	if (top_node->IsDict() && keys_.has_value())
	{
		auto& dict = std::get<Dict>(top_node->GetValue());
		auto it = dict.emplace(keys_.value(), Array());
		nodes_stack_.push_back(&it.first->second);
		keys_.reset();
	}
	else if (top_node->IsArray())
	{
		auto& arr = std::get<Array>(top_node->GetValue());
		arr.emplace_back(Array());
		*(nodes_stack_.back()) = arr;
		nodes_stack_.push_back(&arr.back());
	}
	else if (top_node->IsNull())
	{
		top_node->GetValue() = Array();
	}
	else
		throw std::logic_error("Wrong context of StartArray()");

	return *this;
}

json::Builder& json::Builder::EndArray()
{
	auto top_node = nodes_stack_.back();

	if (top_node->IsArray())
		nodes_stack_.pop_back();
	else
		throw std::logic_error("Array must exist");

	return *this;
}

json::KeyItemContext json::Builder::Key(std::string key)
{
	auto top_node = nodes_stack_.back();

	if (top_node->IsDict() && !keys_.has_value())
		keys_ = std::move(key);
	else
		throw std::logic_error("Wrong map key: " + key);

	return *this;
}

json::Builder& json::Builder::Value(Node::Value value)
{
	auto top_node = nodes_stack_.back();

	if (top_node->IsArray())
	{
		auto arr = top_node->AsArray();
		arr.push_back(GetNode(value));
		*(nodes_stack_.back()) = arr;
	}
	else if (top_node->IsDict() && keys_.has_value())
	{
		auto dict = top_node->AsDict();
		dict[keys_.value()].GetValue() = value;
		*(nodes_stack_.back()) = dict;
		keys_.reset();
	}
	else if (top_node->IsNull())
	{
		root_.GetValue() = value;
	}
	else
		throw std::logic_error("Wrong context of command Value()");
	return *this;
}

json::Node json::Builder::Build()
{
	if (nodes_stack_.size() == 1)
	{
		if (nodes_stack_.back()->IsArray() || nodes_stack_.back()->IsDict() || nodes_stack_.back()->IsNull())
			throw std::logic_error("You must close Array or Dict before Build()");
		else
			return root_;
	}

	return root_;
}

json::KeyItemContext json::DictItemContext::Key(std::string key)
{
	return main_.Key(key);
}

json::Builder& json::DictItemContext::EndDict()
{
	return main_.EndDict();
}

json::ValueItemContext json::KeyItemContext::Value(Node::Value val)
{
	return main_.Value(val);
}

json::ArrayItemContext json::KeyItemContext::StartArray()
{
	return main_.StartArray();
}

json::DictItemContext json::KeyItemContext::StartDict()
{
	return main_.StartDict();
}

json::KeyItemContext json::ValueItemContext::Key(std::string key)
{
	return main_.Key(key);
}

json::Builder& json::ValueItemContext::EndDict()
{
	return main_.EndDict();
}

json::ArrayItemContext json::ArrayItemContext::StartArray()
{
	return main_.StartArray();
}

json::DictItemContext json::ArrayItemContext::StartDict()
{
	return main_.StartDict();
}
json::ArrayItemContext json::ArrayItemContext::Value(Node::Value val)
{
	return main_.Value(val);
}
json::Builder& json::ArrayItemContext::EndArray()
{
	return main_.EndArray();
}

json::Node json::Builder::GetNode(json::Node::Value value) {
	if (std::holds_alternative<int>(value))
		return Node(std::get<int>(value));
	else if (std::holds_alternative<double>(value))
		return Node(std::get<double>(value));
	else if (std::holds_alternative<std::string>(value))
		return Node(std::get<std::string>(value));
	else if (std::holds_alternative<std::nullptr_t>(value))
		return Node(std::get<std::nullptr_t>(value));
	else if (std::holds_alternative<bool>(value))
		return Node(std::get<bool>(value));
	else if (std::holds_alternative<Dict>(value))
		return Node(std::get<Dict>(value));
	else if (std::holds_alternative<Array>(value))
		return Node(std::get<Array>(value));
	else
		throw std::logic_error("Bad value");
}