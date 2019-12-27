using System;
using System.IO;

namespace ImageCheck
{
    class Program
    {
        static bool Compare(FileStream fs, byte[] target)
        {
            var buffer = new byte[target.Length];
            fs.Read(buffer, 0, buffer.Length);
            for (int i = 0; i < target.Length; ++i)
                if (buffer[i] != target[i])
                    return false;
            return true;
        }
        
        static void Main(string[] args)
        {
            foreach (var f in Directory.EnumerateFiles(".\\"))
            {
                if (f.ToLower().EndsWith(".jpg"))
                {
                    using (var fs = File.OpenRead(f))
                    {
                        if (fs.Length > 4)
                        {
                            if (Compare(fs, new byte[] { 0xff, 0xd8 }))
                            {
                                fs.Seek(-2, SeekOrigin.End);
                                if (Compare(fs, new byte[] { 0xff, 0xd9 }))
                                    continue;
                            }
                        }
                        Console.WriteLine(f + " is invalid.");
                    }
                }
                else if (f.ToLower().EndsWith(".png"))
                {
                    using (var fs = File.OpenRead(f))
                    {
                        if (fs.Length > 20)
                        {
                            if (Compare(fs, new byte[] { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A }))
                            {
                                fs.Seek(-12, SeekOrigin.End);
                                if (Compare(fs, new byte[] { 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82 }))
                                    continue;
                            }
                        }
                        Console.WriteLine(f + " is invalid.");
                    }
                }
                else
                {
                    Console.WriteLine(f + " is not a known image type.");
                }
            }
        }
    }
}
