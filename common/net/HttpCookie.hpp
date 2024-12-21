// Created by author ethereal on 2024/12/21.
// Copyright (c) 2024 ethereal. All rights reserved.
#pragma once
#include <chrono>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace common::net
{
class HttpCookie
{
public:
	HttpCookie(std::string name, std::string value) : name_(std::move(name)), value_(std::move(value)) {}

	void setDomain(const std::string& domain) {
		domain_ = domain;
	}

	void setPath(const std::string& path) {
		path_ = path;
	}

	void setSecure(const bool secure) {
		secure_ = secure;
	}

	void setHttpOnly(const bool httpOnly) {
		httpOnly_ = httpOnly;
	}

	void setMaxAge(std::chrono::seconds maxAge) {
		maxAge_ = maxAge;
		expiry_ = std::chrono::system_clock::now() + maxAge;
	}

	void setExpiry(const std::chrono::system_clock::time_point& expiry) {
		expiry_ = expiry;
	}

	[[nodiscard]] std::string getName() const {
		return name_;
	}

	[[nodiscard]] std::string getValue() const {
		return value_;
	}

	[[nodiscard]] std::optional<std::string> getDomain() const {
		return domain_;
	}

	[[nodiscard]] std::optional<std::string> getPath() const {
		return path_;
	}

	[[nodiscard]] bool isSecure() const {
		return secure_;
	}

	[[nodiscard]] bool isHttpOnly() const {
		return httpOnly_;
	}

	[[nodiscard]] std::optional<std::chrono::system_clock::time_point> getExpiry() const {
		return expiry_;
	}

	[[nodiscard]] bool isExpired() const {
		if (expiry_) {
			return std::chrono::system_clock::now() > expiry_.value();
		}
		return false;
	}

	[[nodiscard]] std::string toString() const {
		std::ostringstream oss;
		oss << name_ << "=" << value_;
		if (domain_) oss << "; Domain=" << domain_.value();
		if (path_) oss << "; Path=" << path_.value();
		if (secure_) oss << "; Secure";
		if (httpOnly_) oss << "; HttpOnly";
		if (expiry_) {
			const auto expiry_time_t = std::chrono::system_clock::to_time_t(expiry_.value());
			oss << "; Expires=" << std::put_time(std::gmtime(&expiry_time_t), "%a, %d-%b-%Y %H:%M:%S GMT");
		}
		return oss.str();
	}

	static HttpCookie parse(const std::string& setCookieHeader) {
		std::istringstream stream(setCookieHeader);
		std::string token;
		std::string name, value;
		std::optional<std::string> domain;
		std::optional<std::string> path;
		std::optional<std::chrono::system_clock::time_point> expiry;
		bool secure = false;
		bool httpOnly = false;
		while (std::getline(stream, token, ';')) {
			if (auto pos = token.find('='); pos != std::string::npos) {
				std::string key = token.substr(0, pos);
				std::string val = token.substr(pos + 1);
				if (key == "Domain") {
					domain = val;
				}
				else if (key == "Path") {
					path = val;
				}
				else if (key == "Expires") {
					std::istringstream dateStream(val);
					std::tm tm = {};
					dateStream >> std::get_time(&tm, "%a, %d-%b-%Y %H:%M:%S GMT");
					expiry = std::chrono::system_clock::from_time_t(std::mktime(&tm));
				}
				else if (key.empty()) {
					secure = (val == "Secure");
				}
				else if (key == "HttpOnly") {
					httpOnly = true;
				}
				else {
					if (name.empty()) {
						name = key;
						value = val;
					}
				}
			}
		}
		HttpCookie cookie(name, value);
		if (domain) cookie.setDomain(domain.value());
		if (path) cookie.setPath(path.value());
		if (expiry) cookie.setExpiry(expiry.value());
		cookie.setSecure(secure);
		cookie.setHttpOnly(httpOnly);
		return cookie;
	}

private:
	std::string name_;
	std::string value_;
	std::optional<std::string> domain_;
	std::optional<std::string> path_;
	std::optional<std::chrono::system_clock::time_point> expiry_;
	std::optional<std::chrono::seconds> maxAge_;
	bool secure_ = false;
	bool httpOnly_ = false;
};
}
