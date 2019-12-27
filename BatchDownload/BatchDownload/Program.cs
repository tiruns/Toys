using System;
using System.Net;
using System.IO;
using System.Threading.Tasks;
using System.Threading;
using System.Collections.Generic;

namespace BatchDownload
{
    class App
    {
        readonly List<string> mUrlList;
        readonly string mReferer;
        readonly string mTargetDir;

        public App(List<string> urlList, string referer, string targetDir)
        {
            mUrlList = urlList;
            mReferer = referer;
            mTargetDir = targetDir;

            if (!Directory.Exists(targetDir))
                Directory.CreateDirectory(targetDir);
            if (!Path.EndsInDirectorySeparator(targetDir))
                mTargetDir = $"{mTargetDir}{Path.DirectorySeparatorChar}";
        }

        public async Task Run()
        {
            var tasks = new List<Task>(8);
            for (int i = 0; i < 8; ++i)
                tasks.Add(Task.CompletedTask);
            int finished = -8;

            foreach (var url in mUrlList)
            {
                var task = await Task.WhenAny(tasks);
                int index = tasks.IndexOf(task);
                tasks[index] = DownloadFileAsync(url);
                finished += 1;
                if (finished > 0)
                    Console.WriteLine("{0}/{1}", finished, mUrlList.Count);
            }
            while (tasks.Count != 0)
            {
                var task = await Task.WhenAny(tasks);
                tasks.Remove(task);
                finished += 1;
                if (finished > 0)
                    Console.WriteLine("{0}/{1}", finished, mUrlList.Count);
            }
        }

        public WebClient CreateWebClient()
        {
            var client = new WebClient();
            if (mReferer != null)
                client.Headers.Add(HttpRequestHeader.Referer, mReferer);
            return client;
        }

        public async Task DownloadFileAsync(string url, int retry = 0)
        {
            var idx = url.LastIndexOf('/') + 1;
            string fileName = mTargetDir + url[idx..];

            try
            {
                using var client = CreateWebClient();
                await client.DownloadFileTaskAsync(url, fileName);
            }
            catch (Exception ex) // TODO: fix CA1031
            {
                if (retry < 2)
                {
                    await DownloadFileAsync(url, retry + 1);
                }
                else
                {
                    Console.WriteLine(ex.Message);
                    Console.WriteLine($"Failed to download: {url}");
                }
            }
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length == 0)
            {
                Console.WriteLine("Usage:\t\t.\\BatchDownload.exe UrlList [Referer] [TargetDir=downloads]");
                Console.WriteLine("Examples:\t.\\BatchDownload.exe list.txt");
                Console.WriteLine("\t\t.\\BatchDownload.exe list.txt https://www.github.com/ D:/target/");
                return;
            }

            var urlList = new List<string>();
            string referer = null;
            string targetDir = "downloads";

            if (args.Length > 1)
                referer = args[1];
            if (args.Length > 2)
                targetDir = args[2];

            using (var listFile = new StreamReader(args[0]))
            {
                while (!listFile.EndOfStream)
                {
                    var url = listFile.ReadLine().Trim();
                    if (url.Length != 0)
                        urlList.Add(url);
                }
            }

            var a = new App(urlList, referer, targetDir);
            a.Run().Wait();
        }
    }
}
