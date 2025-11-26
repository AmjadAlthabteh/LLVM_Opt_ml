# AI Debugger Configuration Guide

## What is a Config File?

A configuration file (`.aidebuggerrc`) lets you save your preferences so you don't have to type the same command-line options every time you use AI Debugger.

## Quick Setup

1. **Copy the sample file:**
   ```batch
   REM Windows
   copy .aidebuggerrc.sample .aidebuggerrc

   # Linux/Mac
   cp .aidebuggerrc.sample .aidebuggerrc
   ```

2. **Edit `.aidebuggerrc` with your preferences**

3. **Use it:**
   ```batch
   cli_tool.exe --config .aidebuggerrc crash.log
   ```

That's it! AI Debugger will use your saved settings.

---

## Configuration Options

### Basic Settings

#### `source_directory`
- **What it does**: Tells AI Debugger where your source code is
- **Why you need it**: Required for auto-fix feature
- **Example**: `"./src"` or `"C:/Users/YourName/MyProject/src"`

#### `test_output_directory`
- **What it does**: Where to save generated test files
- **Default**: `"./tests"`
- **Example**: `"./tests"` or `"./test/unit_tests"`

#### `test_framework`
- **What it does**: Which test framework you use
- **Options**: `"gtest"`, `"catch2"`, `"boost"`, `"doctest"`
- **Default**: `"gtest"`

### Behavior Settings

#### `verbose`
- **What it does**: Show detailed information during analysis
- **Values**: `true` or `false`
- **Default**: `false`
- **When to use**: Enable when debugging the debugger or you want more info

#### `auto_test`
- **What it does**: Automatically generate regression tests
- **Values**: `true` or `false`
- **Default**: `false`
- **When to use**: Enable if you always want tests generated

#### `auto_fix`
- **What it does**: Automatically apply the best fix suggestion
- **Values**: `true` or `false`
- **Default**: `false`
- **⚠️ WARNING**: This modifies your source code! Use with caution!

---

## Example Configurations

### 1. Basic Configuration (Recommended for Beginners)

```json
{
  "source_directory": "./src",
  "test_output_directory": "./tests",
  "test_framework": "gtest"
}
```

**Use case**: Simple setup, you manually review all suggestions

### 2. Developer Configuration

```json
{
  "source_directory": "./src",
  "test_output_directory": "./tests",
  "verbose": true,
  "auto_test": true,
  "test_framework": "gtest"
}
```

**Use case**: You want detailed info and always generate tests

### 3. Auto-Fix Configuration (Advanced)

```json
{
  "source_directory": "./src",
  "test_output_directory": "./tests",
  "verbose": true,
  "auto_fix": true,
  "auto_test": true,
  "test_framework": "catch2"
}
```

**Use case**: You trust the AI and want fully automated fixes
**⚠️ WARNING**: Make sure you have git or backups before using auto_fix!

### 4. CI/CD Configuration

```json
{
  "source_directory": "./src",
  "test_output_directory": "./tests",
  "verbose": true,
  "auto_test": true,
  "auto_fix": false,
  "test_framework": "gtest"
}
```

**Use case**: Running in continuous integration, generate tests but don't auto-fix

---

## Where to Put the Config File

You can place `.aidebuggerrc` in:

1. **Project root** (most common):
   ```
   MyProject/
   ├── .aidebuggerrc
   ├── src/
   ├── tests/
   └── ...
   ```

2. **Current directory** (when running the tool)

3. **Anywhere** (specify with `--config` option):
   ```batch
   cli_tool.exe --config /path/to/my-config.json crash.log
   ```

---

## Using the Config File

### Method 1: Automatic (if in current directory)

```batch
cli_tool.exe --config .aidebuggerrc crash.log
```

### Method 2: With custom path

```batch
cli_tool.exe --config C:/configs/ai-debugger-config.json crash.log
```

### Method 3: Override specific options

You can still use command-line options to override config file settings:

```batch
# Config says verbose=false, but this enables it:
cli_tool.exe --config .aidebuggerrc -v crash.log

# Config says auto_fix=true, but you can still disable it:
cli_tool.exe --config .aidebuggerrc crash.log
# (just don't use --auto-fix flag)
```

**Command-line options always take priority over config file!**

---

## Tips & Best Practices

### ✅ DO:

- **Keep it simple**: Start with just `source_directory` and `test_framework`
- **Use version control**: Check `.aidebuggerrc` into git so your team uses the same settings
- **Start conservative**: Set `auto_fix: false` until you trust the tool
- **Use relative paths**: Like `"./src"` instead of `"C:/absolute/path"`

### ❌ DON'T:

- **Enable `auto_fix` without backups**: Always have git or backups first!
- **Use absolute paths unnecessarily**: They won't work for other team members
- **Forget quotes**: JSON requires quotes around strings
- **Include passwords or secrets**: The config file might be committed to git

---

## Sample Workflow

Here's how a typical developer uses config files:

### First Time Setup

1. **Create config**:
   ```bash
   cp .aidebuggerrc.sample .aidebuggerrc
   ```

2. **Edit for your project**:
   ```json
   {
     "source_directory": "./src",
     "test_output_directory": "./tests/regression",
     "verbose": false,
     "auto_test": true,
     "test_framework": "gtest"
   }
   ```

3. **Test it**:
   ```bash
   cli_tool.exe --config .aidebuggerrc demo/null_pointer_crash.txt
   ```

### Daily Usage

Now you just run:
```bash
cli_tool.exe --config .aidebuggerrc crash.log
```

No need to remember all the options!

### Team Setup

**Commit to git**:
```bash
git add .aidebuggerrc
git commit -m "Add AI Debugger config for team"
git push
```

Now everyone on your team has the same settings!

---

## Troubleshooting

### "Config file not found"

**Problem**: The file doesn't exist or path is wrong

**Solution**:
- Check the file exists: `dir .aidebuggerrc` (Windows) or `ls .aidebuggerrc` (Linux/Mac)
- Use absolute path: `--config C:/full/path/to/.aidebuggerrc`
- Make sure you copied from `.aidebuggerrc.sample` and removed `.sample`

### "Failed to parse config file"

**Problem**: Invalid JSON syntax

**Solution**:
- Check for missing commas between fields
- Make sure all strings have quotes: `"value"`
- Remove trailing commas (last item in object can't have comma)
- Use a JSON validator: https://jsonlint.com/

### "Auto-fix not working"

**Problem**: Source files aren't being modified

**Solution**:
- Make sure `source_directory` points to the right place
- Check file permissions (can you write to those files?)
- Verify `auto_fix: true` is set
- Make sure you're using `--auto-fix` flag OR have it in config

---

## Advanced: Multiple Configs

You can have different configs for different situations:

```bash
# Development config (verbose, auto-test)
cli_tool.exe --config .aidebuggerrc.dev crash.log

# Production config (minimal, no auto-fix)
cli_tool.exe --config .aidebuggerrc.prod crash.log

# CI/CD config
cli_tool.exe --config .aidebuggerrc.ci crash.log
```

---

## Complete Reference

Here's every option explained:

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `source_directory` | string | `"."` | Where your source code lives |
| `test_output_directory` | string | `"./tests"` | Where to save generated tests |
| `verbose` | boolean | `false` | Show detailed output |
| `auto_fix` | boolean | `false` | Auto-apply fixes (⚠ modifies code) |
| `auto_test` | boolean | `false` | Auto-generate tests |
| `test_framework` | string | `"gtest"` | Test framework (gtest/catch2/boost/doctest) |

---

## Questions?

- **"Do I need a config file?"**: No, but it's convenient
- **"What if I don't have one?"**: The tool uses sensible defaults
- **"Can I use YAML?"**: No, only JSON format is supported
- **"What about environment variables?"**: Not currently supported

---

**Ready to use config files?** Start with the basic configuration and customize as needed!

For more help, see:
- `GETTING_STARTED.md` - Complete beginner guide
- `README.md` - Full documentation
- `.aidebuggerrc.sample` - Example config file
