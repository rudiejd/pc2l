#ifndef HTTP_FILE_CPP
#define HTTP_FILE_CPP

/**
 * A custom class to streamline sending file contents to a given
 * output stream.  This streams the data in HTTP format.
 *
 * Copyright (C) 2020 raodm@miamioh.edu
 */
#include <string>
#include <iostream>
#include <fstream>
#include "HTTPFile.h"

// Convenience method to determine the content-type of a given file.
std::string http::getContentType(const std::string& path) {
    // Extract the extension from the path.
    const std::string ext = path.substr(path.rfind('.'));
    // Return suitable content types for the different types that we know.
    if (ext == ".html") {
        return "text/html";
    } else if (ext == ".png") {
        return "image/png";
    } else if (ext == ".jpg" || ext == ".jpeg") {
        return "image/jpeg";
    } else if (ext == ".gif") {
        return "image/gif";
    } else if (ext == ".ico") {
        return "image/x-icon";
    }
    // Otherwise return a fixed/default mime type
    return "text/plain";
}

// The operator that HTTP-streams the file if valid or sends a 404 error.
std::ostream& http::operator<<(std::ostream& os, const http::file& file) {
    // First open the data file and if the stream is not good return 404
    std::ifstream data(file.path);
    if (!data.good()) {
        // The file name is invalid. Send HTTP 404 error message.
        const std::string msg = "File not found: " + file.path;
        os << http::Http404Headers;  // Send headers
        os << std::hex << msg.size() << "\r\n" << msg << "\r\n";
    } else {
        // The file is valid. Let's send the 200 header and stream the
        // contents of the file to the client.
        os << file.headers << http::getContentType(file.path) << "\r\n\r\n";
        // Now stream the contents out.
        for (std::string line; std::getline(data, line);) {
            // Note that std::getline() consumes/removes '\n'. So,
            // below we add the '\n' back because that was actually
            // present in the original data and it must be included.
            line += '\n';
            // Write the line of data as an HTTP-chunk
            os << std::hex << line.size() << "\r\n" << line << "\r\n";
        }
    }
    // Finally send the trailing "0" chunk to finish the HTTP-response.
    os << "0\r\n\r\n";
    // Return stream as part of the operator<< API requirement
    return os;
}

# endif
