using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;

// ─── CHALLENGE ────────────────────────────────────────────────────────────────
// Parse a CSV string into a list of typed objects.
// Variants: simple split, quoted fields, header mapping, generic parser.

namespace Recon.CodingChallenges
{
    public class CsvParser
    {
        // ─── Simple: split on comma, no quoted fields ─────────────────────
        public static IEnumerable<string[]> ParseSimple(string csv)
        {
            using var reader = new StringReader(csv);
            string? line;
            while ((line = reader.ReadLine()) is not null)
            {
                if (string.IsNullOrWhiteSpace(line)) continue;
                yield return line.Split(',');
            }
        }

        // ─── With headers: returns Dictionary per row ─────────────────────
        public static IEnumerable<Dictionary<string, string>> ParseWithHeaders(string csv)
        {
            using var reader = new StringReader(csv);
            var headerLine   = reader.ReadLine();
            if (headerLine is null) yield break;

            var headers = headerLine.Split(',').Select(h => h.Trim()).ToArray();
            string? line;

            while ((line = reader.ReadLine()) is not null)
            {
                if (string.IsNullOrWhiteSpace(line)) continue;
                var values = line.Split(',');
                yield return headers.Zip(values, (h, v) => (h, v.Trim()))
                                    .ToDictionary(x => x.h, x => x.v);
            }
        }

        // ─── Quoted field support ─────────────────────────────────────────
        public static IEnumerable<string> ParseLine(string line)
        {
            var fields  = new List<string>();
            var current = new System.Text.StringBuilder();
            bool inQuotes = false;

            for (int i = 0; i < line.Length; i++)
            {
                char c = line[i];

                if (c == '"')
                {
                    if (inQuotes && i + 1 < line.Length && line[i + 1] == '"')
                    {
                        current.Append('"'); i++;  // escaped quote
                    }
                    else inQuotes = !inQuotes;
                }
                else if (c == ',' && !inQuotes)
                {
                    fields.Add(current.ToString().Trim());
                    current.Clear();
                }
                else current.Append(c);
            }
            fields.Add(current.ToString().Trim());
            return fields;
        }

        // ─── Generic: map CSV to typed objects via property names ─────────
        public static IEnumerable<T> Parse<T>(string csv) where T : new()
        {
            using var reader = new StringReader(csv);
            var headerLine   = reader.ReadLine();
            if (headerLine is null) yield break;

            var headers    = headerLine.Split(',').Select(h => h.Trim()).ToArray();
            var properties = typeof(T).GetProperties(BindingFlags.Public | BindingFlags.Instance);
            string? line;

            while ((line = reader.ReadLine()) is not null)
            {
                if (string.IsNullOrWhiteSpace(line)) continue;
                var values = line.Split(',');
                var obj    = new T();

                for (int i = 0; i < headers.Length && i < values.Length; i++)
                {
                    var prop = properties.FirstOrDefault(p =>
                        p.Name.Equals(headers[i], StringComparison.OrdinalIgnoreCase));

                    if (prop is null) continue;
                    var converted = Convert.ChangeType(values[i].Trim(), prop.PropertyType);
                    prop.SetValue(obj, converted);
                }
                yield return obj;
            }
        }
    }

    public class Employee
    {
        public string  Name       { get; set; } = string.Empty;
        public string  Department { get; set; } = string.Empty;
        public decimal Salary     { get; set; }
    }

    class Program
    {
        static void Main()
        {
            var csv = """
                Name,Department,Salary
                Alice,Engineering,120000
                Bob,Marketing,85000
                Carol,Finance,110000
                Dave,Engineering,95000
                """;

            Console.WriteLine("─── Parse with headers (Dictionary) ─────────────");
            foreach (var row in CsvParser.ParseWithHeaders(csv))
                Console.WriteLine($"  {row["Name"],-8} {row["Department"],-15} {row["Salary"]}");

            Console.WriteLine("\n─── Generic parse to Employee objects ───────────");
            foreach (var emp in CsvParser.Parse<Employee>(csv))
                Console.WriteLine($"  {emp.Name,-8} {emp.Department,-15} ${emp.Salary:N0}");

            Console.WriteLine("\n─── Quoted fields ───────────────────────────────");
            var quotedLine = "\"Smith, John\",\"New York, NY\",\"Senior Engineer\"";
            var fields     = CsvParser.ParseLine(quotedLine).ToList();
            Console.WriteLine($"  Fields: {fields.Count}");
            foreach (var f in fields)
                Console.WriteLine($"  '{f}'");

            Console.WriteLine("\n─── Simple parse (no headers) ───────────────────");
            var data = "1,widget,9.99\n2,gadget,49.99\n3,doohickey,24.99";
            foreach (var row in CsvParser.ParseSimple(data))
                Console.WriteLine($"  [{string.Join("] [", row)}]");
        }
    }
}
