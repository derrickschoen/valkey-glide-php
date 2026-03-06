<?php

/**
 * @generate-function-entries
 * @generate-legacy-arginfo
 * @generate-class-entries
 */

/**
 * Script represents a stored Lua script for use with invokeScript.
 *
 * The script source is stored in the GLIDE core cache on construction,
 * and removed when the object is destroyed. Use getHash() to retrieve
 * the SHA1 hash of the stored script.
 */
final class Script
{
    /**
     * Create a new Script instance from Lua source code.
     *
     * @param string $code The Lua script source code
     */
    public function __construct(string $code)
    {
    }

    /**
     * Get the SHA1 hash of the stored script.
     *
     * @return string The SHA1 hash
     */
    public function getHash(): string
    {
    }

    /**
     * Destructor - cleans up the stored script from the cache.
     */
    public function __destruct()
    {
    }
}
