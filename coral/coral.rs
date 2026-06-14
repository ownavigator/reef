#![no_main]

use std::ffi::CStr;

use mimalloc::MiMalloc;

const USAGE_MESSAGE: &str = concat!(
    "Usage:\n",
    "    ",
    env!("CARGO_PKG_NAME"),
    " [OPTION...] <SUBCOMMAND> [ARG...]\n",
    "    ",
    env!("CARGO_PKG_NAME"),
    " [OPTION...] <FILE> [OPTION...]\n",
);

const HELP_MESSAGE: &str = "\
Options:
    -h, --help          Print this help message
    -v, --version       Print version information

Subcommands:
";

#[global_allocator]
static ALLOC: MiMalloc = MiMalloc;

enum OptionType {
    #[allow(unused)]
    Short(char),
    #[allow(unused)]
    Long(&'static str),
    ShortLong(char, &'static str),
}

fn getopt<'opt>(
    option: &'opt str,
    option_type: OptionType,
    out_value: &mut Option<&'opt str>,
) -> bool {
    const OPTION_PREFIX: char = '-';
    const VALUE_SEPARATOR: char = ':';

    let (name, value) = match option.find(VALUE_SEPARATOR) {
        Some(index) => (&option[..index], Some(&option[index + 1..])),
        None => (option, None),
    };

    let matched = if let Some(longname) = name.strip_prefix(OPTION_PREFIX) {
        match option_type {
            OptionType::Short(_) => false,
            OptionType::Long(longopt) | OptionType::ShortLong(_, longopt) => longname == longopt,
        }
    } else {
        match option_type {
            OptionType::Long(_) => false,
            OptionType::Short(shortopt) | OptionType::ShortLong(shortopt, _) => {
                name.len() == 1 && name.as_bytes()[0] == shortopt as u8
            }
        }
    };

    if !matched {
        return false;
    }

    if let Some(value) = value {
        out_value.replace(value);
    }

    true
}

fn print_help<const ERR: bool>() {
    if ERR {
        eprintln!("{USAGE_MESSAGE}{HELP_MESSAGE}");
    } else {
        println!("{USAGE_MESSAGE}{HELP_MESSAGE}")
    }
}

fn print_version() {
    println!("{} {}", env!("CARGO_PKG_NAME"), env!("CARGO_PKG_VERSION"))
}

#[unsafe(no_mangle)]
pub extern "C" fn main(mut argc: isize, argv: *const *const u8) -> isize {
    for i in 1..argc {
        let mut out_value = None;

        let option_raw = unsafe {
            let option_ptr = *argv.offset(i);
            if *option_ptr != b'-' {
                break;
            }

            &CStr::from_ptr(option_ptr.cast()).to_string_lossy()
        };

        if getopt(
            &option_raw[1..],
            OptionType::ShortLong('h', "help"),
            &mut out_value,
        ) {
            print_help::<false>();
            break;
        }

        if getopt(
            &option_raw[1..],
            OptionType::ShortLong('v', "version"),
            &mut out_value,
        ) {
            print_version();
            break;
        }

        argc = argc - 1;
    }

    if argc == 1 {
        print_help::<true>();
        return 1;
    }

    0
}
