namespace Recon.SOLID
{
    // VIOLATION: adding a new payment type requires modifying existing code
    public class PaymentProcessorViolation
    {
        public void Process(string type, decimal amount)
        {
            if (type == "credit") { }
            else if (type == "paypal") { }
            else if (type == "crypto") { }  // requires editing this class every time
        }
    }

    // CORRECT: new payment types extend the system without touching existing code
    public interface IPaymentProvider
    {
        void Process(decimal amount);
    }

    public class CreditCardProvider : IPaymentProvider
    {
        public void Process(decimal amount) { }
    }

    public class PayPalProvider : IPaymentProvider
    {
        public void Process(decimal amount) { }
    }

    public class CryptoProvider : IPaymentProvider
    {
        public void Process(decimal amount) { }
    }

    public class PaymentProcessor
    {
        private readonly IPaymentProvider _provider;

        public PaymentProcessor(IPaymentProvider provider)
            => _provider = provider;

        public void Process(decimal amount)
            => _provider.Process(amount);
    }
}
