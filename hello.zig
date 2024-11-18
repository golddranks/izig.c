const std = @import("std");

fn hoge(piyo: []const u8) void {
    const stdout = std.io.getStdOut().writer();
    try stdout.print("Hello, {s}!\n", .{piyo});
}

pub fn main() !void {
    const stdout = std.io.getStdOut().writer();
    hoge("fuga");
    try stdout.print("Hello, {s}!\n", .{"world"});
}
