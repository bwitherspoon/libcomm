/*
 * Copyright 2015 C. Brett Witherspoon
 */

#ifndef COMM_BLOCK_HPP_
#define COMM_BLOCK_HPP_

namespace comm
{
/**
 * An interface class for signal processing blocks
 */
class block
{
public:
  //! Deconstruct a block
  virtual ~block() { };
  //! Execute a block
  virtual void operator()() = 0;
  //! Returns true of the block is ready to execute
  virtual bool ready() = 0;
};

} /* namespace comm */

#endif /* COMM_BLOCK_HPP_ */
