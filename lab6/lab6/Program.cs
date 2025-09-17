using System;
using System.Collections.Generic;
using System.IO;
using System.Diagnostics;
using System.Security.Cryptography;
using System.Threading.Tasks;
using System.Text;

class Program
{
    static async Task Main(string[] args)
    {
        string inputFilePath = "input.txt";
        string outputFilePath = "output.txt";
        string outputFilePath_par = "output_par.txt";
        TimeSpan time;
        Stopwatch stopwatch = new Stopwatch();

		int num_tasks = 4;

        process_serial(inputFilePath, outputFilePath);
        process_serial(inputFilePath, outputFilePath);
        stopwatch.Start();
        process_serial(inputFilePath, outputFilePath);
        stopwatch.Stop();
        time = stopwatch.Elapsed;
        Console.WriteLine("Serial");
        Console.WriteLine(time);

        stopwatch.Reset();

        await process_parallel(inputFilePath, outputFilePath_par, num_tasks);
        await process_parallel(inputFilePath, outputFilePath_par, num_tasks);
        stopwatch.Start();
        await process_parallel(inputFilePath, outputFilePath_par, num_tasks);
        stopwatch.Stop();
        time = stopwatch.Elapsed;
        Console.WriteLine("Parallel");
        Console.WriteLine(time);
    }

    static void process_serial(string inputFilePath, string outputFilePath) {
        var lines = File.ReadAllLines(inputFilePath);

        Stopwatch stopwatch = new Stopwatch();

        stopwatch.Start();

        var hashes = ComputeMD5(lines, 0, lines.Length);

        stopwatch.Stop();
        TimeSpan time = stopwatch.Elapsed;
        Console.WriteLine("Serial computation");
        Console.WriteLine(time);

        File.WriteAllLines(outputFilePath, hashes);
    }

    static async Task process_parallel(string inputFilePath, string outputFilePath, int num_tasks) {
        var lines = File.ReadAllLines(inputFilePath);
        var tasks = new List<Task<List<string>>>();
        Stopwatch stopwatch = new Stopwatch();

        stopwatch.Start();

        int chunkSize = (int)Math.Ceiling((double)lines.Length / num_tasks);
        for (int i = 0; i < lines.Length; i += chunkSize)
        {
            int start = i;
            int end = Math.Min(i + chunkSize, lines.Length);
            tasks.Add(Task.Run(() => ComputeMD5(lines, start, end)));
        }

		var hashLists = await Task.WhenAll(tasks);
        var hashes = new List<string>();
        foreach (var hashList in hashLists)
        {
            hashes.AddRange(hashList);
        }

        stopwatch.Stop();
        TimeSpan time = stopwatch.Elapsed;
        Console.WriteLine("Parallel computation");
        Console.WriteLine(time);

        File.WriteAllLines(outputFilePath, hashes);
    }

    static List<string> ComputeMD5(string[] lines, int start, int end)
    {
        var hashes = new List<string>();
        using (var md5 = MD5.Create())
        {
            for (int i = start; i < end; i++)
            {
                byte[] inputBytes = Encoding.UTF8.GetBytes(lines[i]);
                byte[] hashBytes = md5.ComputeHash(inputBytes);
                hashes.Add(BitConverter.ToString(hashBytes).Replace("-", "").ToLowerInvariant());
            }
        }
        return hashes;
    }
}
