const std = @import("std");

fn hoge(piyo: []const u8) void {
    const stdout = std.io.getStdOut().writer();
    stdout.print("こんにちは、{s}！\n", .{piyo}) catch {};
}

pub fn main() !void {
    const stdout = std.io.getStdOut().writer();
    hoge("fuga");
    try stdout.print("Nice: {d}\n", .{42 + 69});
}
