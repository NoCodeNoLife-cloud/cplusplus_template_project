// Created by author ethereal on 2024/12/20.
// Copyright (c) 2024 ethereal. All rights reserved.
#pragma once
#include <array>
#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>

namespace common::net
{
/// \brief Represents an IP address, providing methods for address manipulation and information retrieval.
/// \details The InetAddress class supports operations such as obtaining the address in byte form,
/// determining the hostname, checking if the address is a loopback or multicast address,
/// and determining if it is reachable within a specified timeout. It also provides functionality
/// for comparing equality and generating hash codes for InetAddress objects.
class InetAddress
{
public:
	explicit InetAddress(const boost::asio::ip::address_v6::bytes_type& addr);
	explicit InetAddress(const std::string& host);
	[[nodiscard]] auto getAddress() const -> std::vector<unsigned char>;
	[[nodiscard]] auto getHostAddress() const -> std::string;
	static auto getLocalHost() -> InetAddress;
	[[nodiscard]] auto equals(const InetAddress& other) const -> bool;
	[[nodiscard]] auto getCanonicalHostName() const -> std::string;
	[[nodiscard]] auto getHostName() const -> std::string;
	[[nodiscard]] auto hashCode() const -> size_t;
	[[nodiscard]] auto isLoopbackAddress() const -> bool;
	[[nodiscard]] auto isMulticastAddress() const -> bool;
	[[nodiscard]] auto isReachable(int timeout_ms) const -> bool;
	[[nodiscard]] auto toString() const -> std::string;

private:
	boost::asio::ip::address ip_address_;
	boost::asio::io_context io_context_;
};
}
