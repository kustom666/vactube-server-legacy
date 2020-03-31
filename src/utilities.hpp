#ifndef QUOIL_UTILITIES
#define QUOIL_UTILITIES

#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <botan/botan.h>
#include <botan/x509self.h>
#include <botan/rsa.h>
#include <botan/x509_ca.h>
#include <botan/system_rng.h>
#include <botan/pubkey.h>
#include <yaml-cpp/yaml.h>

namespace Quoil {
  /***
   * Generates a private key and a corresponding self signed certificate for the server
   * Args: 
   *  - pkey_filename: the name of the (new) file to save the private key data to   
   *  - cert_filename: the name of the (new) file to save the certificate data to
   */
  void GenCerts(std::string pkey_filename = "key.pem", std::string cert_filename = "cert.pem");

  /**
   * Runs a setup wizard that guides the user through 
   * the configuration of vactube and generates TLS 
   * keys to establish secure communications
   */
  void SetupWizard();

  /**
   * Reads a whole file to a std::string
   * Args:
   *  - filename: the path to the file to read
   */
  std::string ReadFile(const std::string filename);
}

#endif