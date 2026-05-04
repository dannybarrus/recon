namespace Recon.SOLID
{
    // VIOLATION: one class owns order processing, persistence, AND notification
    public class OrderServiceViolation
    {
        public void ProcessOrder(Order order)
        {
            Validate(order);
            SaveToDatabase(order);
            SendConfirmationEmail(order);
            GenerateInvoice(order);
        }

        private void Validate(Order order) { }
        private void SaveToDatabase(Order order) { }
        private void SendConfirmationEmail(Order order) { }
        private void GenerateInvoice(Order order) { }
    }

    // CORRECT: each class has exactly one reason to change
    public class OrderValidator
    {
        public bool Validate(Order order) => order.Total > 0;
    }

    public class OrderRepository
    {
        public void Save(Order order) { }
    }

    public class OrderNotifier
    {
        public void SendConfirmation(Order order) { }
    }

    public class InvoiceGenerator
    {
        public void Generate(Order order) { }
    }

    // Coordinator — thin orchestration only, no business logic
    public class OrderProcessor
    {
        private readonly OrderValidator _validator;
        private readonly OrderRepository _repository;
        private readonly OrderNotifier _notifier;
        private readonly InvoiceGenerator _invoiceGenerator;

        public OrderProcessor(
            OrderValidator validator,
            OrderRepository repository,
            OrderNotifier notifier,
            InvoiceGenerator invoiceGenerator)
        {
            _validator = validator;
            _repository = repository;
            _notifier = notifier;
            _invoiceGenerator = invoiceGenerator;
        }

        public void Process(Order order)
        {
            if (!_validator.Validate(order)) return;
            _repository.Save(order);
            _notifier.SendConfirmation(order);
            _invoiceGenerator.Generate(order);
        }
    }

    public class Order
    {
        public int Id { get; set; }
        public decimal Total { get; set; }
    }
}
