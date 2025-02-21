// crypto_cli.cpp
#include "crypto.hpp"
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <fstream>
#include <gflags/gflags.h>
#include <iostream>

// Command flags
DEFINE_string(mode, "", "Operation mode: 'encrypt' or 'decrypt'");
DEFINE_string(input, "", "Input file path");
DEFINE_string(output, "", "Output file path");
DEFINE_string(key_file, "", "Path to key file (optional)");
DEFINE_string(iv_file, "", "Path to IV file (optional)");
DEFINE_bool(generate_key, false, "Generate new key and IV");
DEFINE_string(save_key, "", "Path to save generated key (optional)");
DEFINE_string(save_iv, "", "Path to save generated IV (optional)");
DEFINE_bool(show_hash, false, "Show file hash after operation");
DEFINE_string(commit, "",
              "Use commit string as key (40 characters SHA-1 hash)");

// Validation functions
static bool ValidateMode(const char *flagname, const std::string &value) {
  if (value != "encrypt" && value != "decrypt" && value != "") {
    std::cout << "Invalid mode. Must be 'encrypt' or 'decrypt'" << std::endl;
    return false;
  }
  return true;
}

static bool ValidateInputFile(const char *flagname, const std::string &value) {
  if (!value.empty()) {
    std::ifstream file(value);
    if (!file.good()) {
      std::cout << "Input file does not exist: " << value << std::endl;
      return false;
    }
  }
  return true;
}

static bool ValidateCommit(const char *flagname, const std::string &value) {
  if (!value.empty() && value.length() != 40) {
    std::cout << "Commit hash must be 40 characters long" << std::endl;
    return false;
  }
  return true;
}

// Register validators
DEFINE_validator(mode, &ValidateMode);
DEFINE_validator(input, &ValidateInputFile);
DEFINE_validator(commit, &ValidateCommit);

// Utility functions
bool readFileToString(const std::string &filepath, std::string &content) {
  try {
    std::ifstream file(filepath, std::ios::binary);
    if (!file)
      return false;

    file.seekg(0, std::ios::end);
    content.resize(file.tellg());
    file.seekg(0);
    file.read(&content[0], content.size());
    return true;
  } catch (const std::exception &e) {
    std::cerr << "Error reading file: " << e.what() << std::endl;
    return false;
  }
}

bool writeStringToFile(const std::string &filepath,
                       const std::string &content) {
  try {
    std::ofstream file(filepath, std::ios::binary);
    if (!file)
      return false;

    file.write(content.data(), content.size());
    return true;
  } catch (const std::exception &e) {
    std::cerr << "Error writing file: " << e.what() << std::endl;
    return false;
  }
}

void printUsage() {
  std::cout
      << "Crypto CLI Tool Usage:\n"
      << "  Required flags:\n"
      << "    --mode=<encrypt|decrypt>   Operation mode\n"
      << "    --input=<path>            Input file path\n"
      << "    --output=<path>           Output file path\n"
      << "\n"
      << "  Key generation options (choose one):\n"
      << "    1. --generate_key         Generate new key and IV\n"
      << "    2. --key_file=<path> and --iv_file=<path>  Use existing key/IV "
         "files\n"
      << "    3. --commit=<hash>        Use git commit hash as key (40 chars)\n"
      << "\n"
      << "  Optional flags:\n"
      << "    --save_key=<path>         Save generated key to file\n"
      << "    --save_iv=<path>          Save generated IV to file\n"
      << "    --show_hash               Show file hash after operation\n"
      << std::endl;
}

int main(int argc, char *argv[]) {
  gflags::SetUsageMessage("Crypto CLI Tool");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  if (FLAGS_mode.empty() || FLAGS_input.empty() || FLAGS_output.empty()) {
    printUsage();
    return 1;
  }

  try {
    infer::encrypt::CryptoConfig config;

    if (FLAGS_generate_key) {
      if (!FLAGS_commit.empty() || !FLAGS_key_file.empty() ||
          !FLAGS_iv_file.empty()) {
        std::cerr << "Cannot combine --generate_key with other key options"
                  << std::endl;
        return 1;
      }
      config = infer::encrypt::Crypto::generateSecureConfig();

    } else if (!FLAGS_commit.empty()) {
      if (!FLAGS_key_file.empty() || !FLAGS_iv_file.empty()) {
        std::cerr << "Cannot combine --commit with key file options"
                  << std::endl;
        return 1;
      }
      config = infer::encrypt::Crypto::deriveKeyFromCommit(FLAGS_commit);

    } else if (!FLAGS_key_file.empty() && !FLAGS_iv_file.empty()) {
      if (!readFileToString(FLAGS_key_file, config.key) ||
          !readFileToString(FLAGS_iv_file, config.iv)) {
        std::cerr << "Failed to read key or IV file" << std::endl;
        return 1;
      }
    } else {
      std::cerr << "Must specify one of: --generate_key, --commit, or both "
                   "--key_file and --iv_file"
                << std::endl;
      return 1;
    }

    if (!FLAGS_save_key.empty()) {
      writeStringToFile(FLAGS_save_key, config.key);
    }
    if (!FLAGS_save_iv.empty()) {
      writeStringToFile(FLAGS_save_iv, config.iv);
    }

    infer::encrypt::Crypto crypto(config);
    bool success = false;

    if (FLAGS_mode == "encrypt") {
      success = crypto.encryptFile(FLAGS_input, FLAGS_output);
    } else {
      success = crypto.decryptFile(FLAGS_input, FLAGS_output);
    }

    if (!success) {
      std::cerr << "Operation failed" << std::endl;
      return 1;
    }

    std::cout << "Operation completed successfully" << std::endl;

    if (FLAGS_show_hash) {
      std::string hash =
          infer::encrypt::Crypto::calculateFileHash(FLAGS_output);
      std::cout << "Output file hash (SHA256): " << hash << std::endl;
    }

  } catch (const infer::encrypt::CryptoException &e) {
    std::cerr << "Crypto error: " << e.what() << std::endl;
    return 1;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}